#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <libssh/libssh.h>
#include <stdlib.h>
#include <stdio.h>

#include <nlohmann/json.hpp>
#include <cpp-httplib/httplib.h>
#include "rpc-embedding.hpp"
#include "storage.hpp"

using json = nlohmann::json;

pid_t llama_pid = -1;
std::vector<std::string> rpc_devices;
std::string model = "ggml-org/gemma-3-1b-it-GGUF"; // Modelo por defecto

static void stop_background_llama() {
    if(llama_pid != -1) {
        std::cout << "Stopping background process with PID: " << llama_pid << std::endl;
        kill(llama_pid, SIGTERM);

        // Esperar a que el proceso hijo termine
        int status;
        waitpid(llama_pid, &status, 0);
        llama_pid = -1;
    }
}


static void start_background_llama() {

    stop_background_llama();

    pid_t pid = fork();

    if (pid == 0) {
        // Proceso hijo
        std::vector<char*> args;
        args.push_back((char*)"./llama-server");
        args.push_back((char*)"-hf");
        args.push_back((char*)model.c_str());
        args.push_back((char*)"-c");
        args.push_back((char*)"2048");
        if (rpc_devices.size() > 0) {
            args.push_back((char*)"--rpc");
            for (int i = 0; i < rpc_devices.size(); i++) {
                args.push_back((char*)rpc_devices[i].c_str());
            }
        }
        args.push_back((char*)"--host");
        args.push_back((char*)"0.0.0.0");
        args.push_back((char*)"--port");
        args.push_back((char*)"8080");
        args.push_back(NULL);

        execvp(args[0], args.data());

        perror("execvp failed");
        exit(1);
    } else if (pid > 0) {
        // Proceso padre 
        llama_pid = pid;
        std::cout << "New process started with PID: " << pid << std::endl;
    } else {
        perror("Fork failed");
    }
}

static std::string connect_ssh(std::string ip, std::string user, std::string password) {
    ssh_session session;
    ssh_channel channel;
    int rc;
    char buffer[256];
    int nbytes;
    // --- CONFIGURACIÓN ---
    const char* rpi_ip = ip.c_str();
    const char* rpi_user = user.c_str();
    const char* rpi_pass = password.c_str();
    // Comando que hay que mejorar metiendo un .sh y que haga ahí todas las comprobaciones 
    /*const char* comando = "sh -c 'if [ $(id -u) -eq 0 ] || groups | grep -q docker; "
                        "then CMD=\"docker\"; else CMD=\"sudo docker\"; fi; "
                        "$CMD run -d --rm -p 50051:50051 --name llama-worker llama-worker";
    */

    const char* comando = "sudo docker run -d --rm -p 50051:50051 --name llama-worker alvaropereda/llama-worker";
    // 1. Iniciar sesión
    session = ssh_new();
    if (session == NULL) return "Error configuring ssh";

    ssh_options_set(session, SSH_OPTIONS_HOST, rpi_ip);
    ssh_options_set(session, SSH_OPTIONS_USER, rpi_user);

    // Desactivar chequeo estricto de llaves (Solo para pruebas)
    int strict_check = 0;
    ssh_options_set(session, SSH_OPTIONS_STRICTHOSTKEYCHECK, &strict_check);

    // 2. Conectar
    std::cout << "Connecting to " << rpi_ip << "..." << std::endl;
    rc = ssh_connect(session);
    if (rc != SSH_OK) {
        std::cerr << "Error with the connection: " << ssh_get_error(session) << std::endl;
        ssh_free(session);
        return "SSH connection could not be established to the remote host";
    }

    // 3. Autenticar (Password)
    std::cout << "Authenticating..." << std::endl;
    rc = ssh_userauth_password(session, NULL, rpi_pass);
    if (rc != SSH_AUTH_SUCCESS) {
        std::cerr << "Error with the authentication: " << ssh_get_error(session) << std::endl;
        ssh_disconnect(session);
        ssh_free(session);
        return "Invalid username or password for the provided user account";
    }

    // 4. Ejecutar comando
    std::cout << "Running: " << comando << std::endl;
    channel = ssh_channel_new(session);
    rc = ssh_channel_open_session(channel);
    rc = ssh_channel_request_exec(channel, comando);

    // 5. Leer respuesta
    while ((nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0)) > 0) {
        if (fwrite(buffer, 1, nbytes, stdout) != (size_t)nbytes) {
            ssh_channel_close(channel);
            ssh_channel_free(channel);
            return "Error while retrieving command output from remote host";
        }
    }

    // 6. Limpieza
    ssh_channel_send_eof(channel);
    ssh_channel_close(channel);
    ssh_channel_free(channel);
    ssh_disconnect(session);
    ssh_free(session);

    return "";
}

static bool wait_for_server_ready() {
    httplib::Client cli("localhost", 8080);
    cli.set_connection_timeout(0, 500000); // 0.5 segundos timeout conexión

    int max_retries = 1200; // Son 10 minutos

    for (int i = 0; i < max_retries; i++) {
        int status;
        pid_t result = waitpid(llama_pid, &status, WNOHANG);
        if (result == llama_pid) {
            std::cerr << "Error: llama-server process terminated unexpectedly." << std::endl;
            return false;
        }

        if (auto res = cli.Get("/health")) {
            if (res->status == 200) {
                std::cout << "Server is UP and Ready!" << std::endl;
                return true;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        if (i % 10 == 0) std::cout << "Waiting for model to load..." << std::endl;
    }

    std::cerr << "Error: Timeout waiting for server to start." << std::endl;
    return false;

}

int main() {
    httplib::Server svr;

    svr.Get("/health", [](const httplib::Request&, httplib::Response& res) {
        res.status = 200;
        res.set_content("ok", "text/plain");
    });

    svr.Get("/documents", [](const httplib::Request&, httplib::Response& res) {
        std::vector<std::string> documents = get_documents();
        json response = documents;
        res.set_content(response.dump(), "application/json");
    });

    svr.Post("/reload", [](const httplib::Request& req, httplib::Response& res) {
        try
        {
            auto body = json::parse(req.body);
            
            if (body.contains("model")) {
                std::string model = body["model"];

                ::model = model;

                start_background_llama();

                if (wait_for_server_ready()) {
                    res.status = 200;
                    res.set_content("ok", "text/plain");
                } else {
                    res.status = 500;
                    res.set_content("{\"error\": \"Model failed to load or timeout\"}", "application/json");
                }
            } else {
                res.status = 400;
                res.set_content("{\"error\": \"The model attribute is missing\"}", "application/json");
            }
        }
        catch(...)
        {
            res.status = 400;
            res.set_content("{\"error\": \"Invalid JSON\"}", "application/json");
        }
    });

    svr.Post("/connect", [](const httplib::Request& req, httplib::Response& res) {
        try
        {
            auto body = json::parse(req.body);
            
            if (body.contains("ip") || body.contains("user") || body.contains("password")) {
                std::string ip = body["ip"];
                std::string user = body["user"];
                std::string password = body["password"];
                std::string message = connect_ssh(ip, user, password);
                std::string ip_with_port = body["ip"].get<std::string>() + ":50051";

                if (message != "") {
                res.status = 400;
                res.set_content(json({{"error", message}}).dump(), "application/json");
                } else {
                    rpc_devices.push_back(ip_with_port);
                    res.status = 200;
                    res.set_content("ok", "text/plain");
                }
            } else {
                res.status = 400;
                res.set_content("{\"error\": \"The device attribute is missing\"}", "application/json");
            }
        }
        catch(...)
        {
            res.status = 400;
            res.set_content("{\"error\": \"Invalid JSON\"}", "application/json");
        }
    });

    svr.Post("/chat/completions", [](const httplib::Request& req, httplib::Response& res) {
        try
        {
            auto body = json::parse(req.body);
            if (body.contains("prompt") || body.contains("use_rag")) {
                std::string prompt = body["prompt"];
                bool use_rag = body.value("use_rag", false);

                std::string context = "";

                if (use_rag) {
                    std::vector<RetrievedChunk> rag_result = generate_embeddings(prompt);
                    for (const RetrievedChunk& chunk : rag_result)
                        context += chunk.content + "\n\n";
                }

                std::cout << "Received context: " << context << std::endl;

                json messages = json::array();

                if (!context.empty()) {
                    messages.push_back({{"role", "system"}, {"content", "Usa el siguiente contexto para responder:\n\n" + context}});
                }
                messages.push_back({{"role", "user"}, {"content", prompt}});

                json llama_body = {
                    {"messages", messages},
                    {"temperature", 0.7},
                    {"stream", true}
                };
                std::string llama_body_str = llama_body.dump();

                res.set_header("Content-Type", "text/event-stream");
                res.set_header("Cache-Control", "no-cache");
                res.set_header("Connection", "keep-alive");
                res.set_header("Access-Control-Allow-Origin", "*");

                res.set_chunked_content_provider(
                    "text/event-stream",
                    [llama_body_str](size_t /*offset*/, httplib::DataSink& sink) {
                        httplib::Client cli("backend", 8080);
                        cli.set_read_timeout(120, 0);

                        auto content_receiver = [&](const char *data, size_t data_length) {
                            if (!sink.write(data, data_length)) {
                                return false;
                            }
                            return true;
                        };

                        httplib::Headers headers = {
                            {"Accept", "text/event-stream"}
                        };

                        auto result = cli.Post(
                            "/chat/completions",
                            headers,
                            llama_body_str,
                            "application/json",
                            content_receiver
                        );

                        if (!result || result->status != 200) {
                            std::cerr << "Error en proxy: "
                                    << (result ? std::to_string(result->status) : "Fallo de conexión")
                                    << std::endl;
                        }
                        sink.done(); // Se devuelve al frontend
                        return true;
                    }
                );
            } else {
                res.status = 400;
                res.set_content("Missing file", "text/plain");
            }
        }
        catch(...)
        {
            res.status = 400;
            res.set_content("{\"error\": \"Invalid JSON\"}", "application/json");
        }

    });

    svr.Post("/document", [](const httplib::Request& req, httplib::Response& res) {
        if (req.form.has_file("file")) {
            httplib::FormData file = req.form.get_file("file");
            std::cout << "File name: " << file.filename << std::endl;
            std::cout << "File size: " << file.content.size() << std::endl;

            generate_embeddings(file.filename, file.content);

            res.set_content("File received", "text/plain");

        } else {
            res.status = 400;
            res.set_content("Missing file", "text/plain");
        }
    });

    svr.Delete("/document", [](const httplib::Request& req, httplib::Response& res) {
        std::cout << "Received request to delete document" << std::endl;
        try
        {
            auto body = json::parse(req.body);

            if (body.contains("document_name")) {
                std::string document_name = body["document_name"];
                delete_document(document_name);
                res.status = 200;
                res.set_content("ok", "text/plain");
            }
            else {
                res.status = 400;
                res.set_content("{\"error\": \"The document_name attribute is missing\"}", "application/json");
            }
        }
        catch(...)
        {
            res.status = 400;
            res.set_content("{\"error\": \"Invalid JSON\"}", "application/json");
        }
        
    });

    std::cout << "Starting server on port 5000..." << std::endl;
    svr.listen("0.0.0.0", 5000);
    return 0;
}
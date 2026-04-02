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
#include "ssh-manager.hpp"

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
        std::vector<const char*> args;
        args.push_back("./llama-server");
        args.push_back("-hf");
        args.push_back(model.c_str());
        args.push_back("-c");
        args.push_back("2048");
        if (!rpc_devices.empty()) {
            args.push_back("--rpc");
            for (const auto& device : rpc_devices) {
                args.push_back(device.c_str());
            }
        }
        args.push_back("--host");
        args.push_back("0.0.0.0");
        args.push_back("--port");
        args.push_back("8080");
        args.push_back(nullptr);

        execvp(args[0], const_cast<char* const*>(args.data()));

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
    Storage rag_storage;

    svr.Get("/health", [](const httplib::Request&, httplib::Response& res) {
        res.status = 200;
        res.set_content("ok", "text/plain");
    });

    svr.Get("/documents", [&rag_storage](const httplib::Request&, httplib::Response& res) {
        std::vector<std::string> documents = rag_storage.get_documents();
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
            
            if (body.contains("ip") && body.contains("user") && body.contains("password")) {
                std::string ip = body["ip"];
                std::string user = body["user"];
                std::string password = body["password"];
                connect_device_ssh(ip.c_str(), user.c_str(), password.c_str());
                std::string ip_with_port = body["ip"].get<std::string>() + ":50051";

                rpc_devices.push_back(ip_with_port);
                res.status = 200;
                res.set_content("ok", "text/plain");
            } else {
                res.status = 400;
                res.set_content("{\"error\": \"The device attribute is missing\"}", "application/json");
            }
        }
        catch(const std::runtime_error& e) {
            res.status = 400;
            res.set_content(json({{"error", e.what()}}).dump(), "application/json");
        }
        catch(...) {
            res.status = 500;
            res.set_content(json({{"error", "An unexpected error occurred on the server"}}).dump(), "application/json");
        }
    });

    svr.Post("/chat/completions", [&rag_storage](const httplib::Request& req, httplib::Response& res) {
        try
        {
            auto body = json::parse(req.body);
            if (body.contains("prompt") || body.contains("use_rag")) {
                std::string prompt = body["prompt"];
                bool use_rag = body.value("use_rag", false);

                std::string context = "";

                if (use_rag) {
                    std::vector<RetrievedChunk> rag_result = generate_embeddings(rag_storage, prompt);
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

    svr.Post("/document", [&rag_storage](const httplib::Request& req, httplib::Response& res) {
        if (req.form.has_file("file")) {
            httplib::FormData file = req.form.get_file("file");
            std::cout << "File name: " << file.filename << std::endl;
            std::cout << "File size: " << file.content.size() << std::endl;

            generate_embeddings(rag_storage, file.filename, file.content);

            res.set_content("File received", "text/plain");

        } else {
            res.status = 400;
            res.set_content("Missing file", "text/plain");
        }
    });

    svr.Delete("/document", [&rag_storage](const httplib::Request& req, httplib::Response& res) {
        std::cout << "Received request to delete document" << std::endl;
        try
        {
            auto body = json::parse(req.body);

            if (body.contains("document_name")) {
                std::string document_name = body["document_name"];
                rag_storage.delete_document(document_name);
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
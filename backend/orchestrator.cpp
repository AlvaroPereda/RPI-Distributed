#include <iostream>
#include <nlohmann/json.hpp>
#include <cpp-httplib/httplib.h>

#include "rpc-embedding.hpp"
#include "storage.hpp"
#include "ssh-manager.hpp"
#include "llama-manager.hpp"

using json = nlohmann::json;

int main() {
    httplib::Server svr;
    Storage rag_storage;
    Llama_manager llama;

    // Gestión de modelos
    svr.Post("/reload", [&llama](const httplib::Request& req, httplib::Response& res) {
        try
        {
            auto body = json::parse(req.body);
            
            if (body.contains("model")) {
                std::string model = body["model"];

                llama.set_model(model);
                llama.start();
                llama.wait_for_ready();

                res.status = 200;
                res.set_content("ok", "text/plain");
            } else {
                res.status = 400;
                res.set_content(json({{"error", "The model attribute is missing"}}).dump(), "application/json");
            }
        } 
        catch (const std::runtime_error& e) {
            res.status = 400;
            res.set_content(json({{"error", e.what()}}).dump(), "application/json");
        }
        catch (...) {
            res.status = 500;
            res.set_content(json({{"error", "An unexpected error occurred on the server"}}).dump(), "application/json");
        }
    });

    // Gestión de dispositivos 
    svr.Post("/connect", [&llama](const httplib::Request& req, httplib::Response& res) {
        try
        {
            auto body = json::parse(req.body);
            
            if (body.contains("ip") && body.contains("user") && body.contains("password")) {
                std::string ip = body["ip"];
                std::string user = body["user"];
                std::string password = body["password"];
                connect_device_ssh(ip.c_str(), user.c_str(), password.c_str());
                std::string ip_with_port = body["ip"].get<std::string>() + ":50051";

                llama.set_rpc_device(ip_with_port);

                res.status = 200;
                res.set_content("ok", "text/plain");
            } else {
                res.status = 400;
                res.set_content(json({{"error", "Missing required fields: ip, user or password"}}).dump(), "application/json");
            }
        }
        catch (const std::runtime_error& e) {
            res.status = 400;
            res.set_content(json({{"error", e.what()}}).dump(), "application/json");
        }
        catch (...) {
            res.status = 500;
            res.set_content(json({{"error", "An unexpected error occurred on the server"}}).dump(), "application/json");
        }
    });

    // RAG
    svr.Get("/documents", [&rag_storage](const httplib::Request&, httplib::Response& res) {
        std::vector<std::string> documents = rag_storage.get_documents();
        res.set_content(json(documents).dump(), "application/json");
    });

    svr.Post("/document", [&rag_storage](const httplib::Request& req, httplib::Response& res) {
        if (req.form.has_file("file")) {
            httplib::FormData file = req.form.get_file("file");
            std::cout << "File name: " << file.filename << std::endl;
            std::cout << "File size: " << file.content.size() << std::endl;
            generate_embeddings(rag_storage, file.filename, file.content);
            res.status = 200;
            res.set_content("ok", "text/plain");
        } else {
            res.status = 400;
            res.set_content(json({{"error", "The file attribute is missing"}}).dump(), "application/json");
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
                res.set_content(json({{"error", "The document_name attribute is missing"}}).dump(), "application/json");
            }
        }
        catch (...) {
            res.status = 500;
            res.set_content(json({{"error", "An unexpected error occurred on the server"}}).dump(), "application/json");
        }
        
    });
    
    // Inferencia
    svr.Post("/chat/completions", [&rag_storage](const httplib::Request& req, httplib::Response& res) {
        try
        {
            auto body = json::parse(req.body);
            if (body.contains("prompt")) {
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
                res.set_content(json({{"error", "The prompt attribute is missing"}}).dump(), "application/json");
            }
        }
        catch (...) {
            res.status = 500;
            res.set_content(json({{"error", "An unexpected error occurred on the server"}}).dump(), "application/json");
        }

    });

    std::cout << "Starting server on port 5000..." << std::endl;
    svr.listen("0.0.0.0", 5000);
    return 0;
}
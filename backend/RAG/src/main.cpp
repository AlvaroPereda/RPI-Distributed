#include <iostream>

#include "json.hpp"
#include "httplib.h"
#include "rpc-embedding.hpp"

using json = nlohmann::json;

int main() {

    httplib::Server svr;

    svr.Post("/chat/completions", [](const httplib::Request& req, httplib::Response& res) {
        try
        {
            auto body = json::parse(req.body);
            if (body.contains("prompt") || body.contains("use_rag")) {
                std::string prompt = body["prompt"];
                bool use_rag = body.value("use_rag", false);

                std::string context = "";
                if (use_rag) {
                    RagResult rag_result = generate_embeddings(prompt)[0];
                    context = rag_result.content;
                }

                std::string enriched_prompt = prompt;
                if (!context.empty()) {
                    enriched_prompt = "Contexto relevante:\n" + context + "\n\n---\nPregunta: " + prompt;
                }

                json llama_body = {
                    {"messages", enriched_prompt},
                    {"temperature", 0,7},
                    {"stream", true}
                };
                std::string llama_body_str = llama_body.dump();

                res.set_header("Content-Type", "text/event-stream");
                res.set_header("Cache-Control", "no-cache");
                res.set_header("Connection", "keep-alive");
                res.set_header("Access-Control-Allow-Origin", "*");

                httplib::Client llama_client("localhost", 8000);
                llama_client.set_read_timeout(60, 0);

                res.set_chunked_content_provider(
                    "text/event-stream",
                    [llama_body_str](size_t /*offset*/, httplib::DataSink& sink) {
                        httplib::Client inner_client("localhost", 8000);
                        inner_client.set_read_timeout(60, 0);

                        auto stream_handle = inner_client.open_stream(
                            "POST",
                            "/chat/completions",
                            {},
                            {},
                            llama_body_str,
                            "application/json"
                        );

                        if (stream_handle.error != httplib::Error::Success) {
                            sink.done();
                            return false;
                        }

                        char buf[4096];
                        while (true) {
                            auto bytes_read = stream_handle.read(buf, sizeof(buf));
                            if (bytes_read <= 0) break;
                            if (!sink.write(buf, bytes_read)) break;
                        }

                        sink.done();
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

    std::cout << "Starting server on port 6000..." << std::endl;
    svr.listen("0.0.0.0", 6000);
    return 0;
}
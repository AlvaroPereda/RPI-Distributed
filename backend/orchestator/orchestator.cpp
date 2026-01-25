#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

#include "httplib.h"
#include "json.hpp"

using json = nlohmann::json;

pid_t llama_pid = -1;

void stop_background_llama() {
    if(llama_pid != -1) {
        std::cout << "Stopping background process with PID: " << llama_pid << std::endl;
        kill(llama_pid, SIGTERM);

        // Esperar a que el proceso hijo termine
        int status;
        waitpid(llama_pid, &status, 0);
        llama_pid = -1;
    }
}


void start_background_llama(std::string model) {

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

int main() {
    httplib::Server svr;

    svr.Get("/health", [](const httplib::Request& req, httplib::Response& res) {
        res.status = 200;
        res.set_content("ok", "text/plain");
    });

    svr.Post("/reload", [](const httplib::Request& req, httplib::Response& res) {
        try
        {
            auto body = json::parse(req.body);
            
            if (body.contains("model")) {
                std::string model = body["model"];

                start_background_llama(model);

                res.status = 200;
                res.set_content("funcionó", "text/plain");
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

    std::cout << "Starting server on port 5000..." << std::endl;
    svr.listen("0.0.0.0", 5000);

    return 0;
}
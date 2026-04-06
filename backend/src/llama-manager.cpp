#include "llama-manager.hpp"

#include <chrono>
#include <iostream>
#include <thread>

#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cpp-httplib/httplib.h>

static constexpr int LLAMA_PORT = 8080;
static constexpr int LLAMA_CONTEXT = 2048;
static constexpr int READY_MAX_RETRIES = 1200; // 10 minutos
static constexpr int READY_INTERVAL_MS = 500;
static constexpr int READY_LOG_INTERVAL = 10;
static const char* LLAMA_HOST = "0.0.0.0";

Llama_manager::~Llama_manager() { stop(); }

void Llama_manager::start() {
    stop();

    pid_t pid = fork();

    if (pid == 0) {
        // Proceso hijo
        std::string ctx  = std::to_string(LLAMA_CONTEXT);
        std::string port = std::to_string(LLAMA_PORT);
        std::vector<const char*> args;
        args.push_back("./llama-server");
        args.push_back("-hf");args.push_back(model.c_str());
        args.push_back("-c");args.push_back(ctx.c_str());
        if (!rpc_devices.empty()) {
            args.push_back("--rpc");
            for (const auto& device : rpc_devices) {
                args.push_back(device.c_str());
            }
        }
        args.push_back("--host");args.push_back(LLAMA_HOST);
        args.push_back("--port");args.push_back(port.c_str());
        args.push_back(nullptr);

        execvp(args[0], const_cast<char* const*>(args.data()));
        std::cerr << "execvp failed: " << strerror(errno) << std::endl;
        _exit(1);
    } else if (pid > 0) {
        // Proceso padre
        llama_pid = pid;
        std::cout << "New process started with PID: " << pid << std::endl;
    } else {
        throw std::runtime_error("Failed to fork llama-server process: " + std::string(strerror(errno)));
    }
}

void Llama_manager::stop() {
    if(llama_pid != -1) {
        std::cout << "Stopping background process with PID: " << llama_pid << std::endl;
        kill(llama_pid, SIGTERM);

        // Esperar a que el proceso hijo termine
        int status;
        waitpid(llama_pid, &status, 0);
        llama_pid = -1;
    }
}

void Llama_manager::wait_for_ready()
{
    httplib::Client cli("localhost", LLAMA_PORT);
    cli.set_connection_timeout(0, 500000); // 0.5 segundos timeout conexión

    for (int i = 0; i < READY_MAX_RETRIES; i++) {
        int status;
        pid_t result = waitpid(llama_pid, &status, WNOHANG);
        if (result == llama_pid) {
            throw std::runtime_error("llama-server process terminated unexpectedly");
        }

        if (auto res = cli.Get("/health")) {
            if (res->status == 200) {
                std::cout << "Server is UP and Ready!" << std::endl;
                return;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(READY_INTERVAL_MS));
        if (i % READY_LOG_INTERVAL == 0) std::cout << "Waiting for model to load..." << std::endl;
    }

    throw std::runtime_error("Timeout waiting for llama-server to start");
}

void Llama_manager::set_rpc_device(const std::string &device) {
    rpc_devices.push_back(device);
}

void Llama_manager::set_model(const std::string &model) {
    this->model = model;
}

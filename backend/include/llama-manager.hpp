#ifndef LLAMA_MANAGER_HPP
#define LLAMA_MANAGER_HPP

#include <string>
#include <vector>
#include <sys/types.h>

class Llama_manager {
    private:
        pid_t llama_pid = -1;
        std::string model = "ggml-org/gemma-3-1b-it-GGUF";
        std::vector<std::string> rpc_devices;
    public:
        Llama_manager() = default;
        ~Llama_manager();
        void start();
        void stop();
        void wait_for_ready();
        void set_rpc_device(const std::string &device);
        void set_model(const std::string& model);
};

#endif
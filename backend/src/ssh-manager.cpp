#include <iostream>
#include <libssh/libssh.h>

#include "ssh-manager.hpp"
class SSH_manager {
    private:
        ssh_session session = nullptr;
        ssh_channel channel = nullptr;
        const char* ip;
        const char* user;
        const char* password;

    public:
        SSH_manager(const char* ip, const char* user, const char* password) : ip(ip), user(user), password(password) {
            session = ssh_new();
        }

        ~SSH_manager() {
            if (channel) {
                ssh_channel_send_eof(channel);
                ssh_channel_close(channel);
                ssh_channel_free(channel);
            }
            if (session) {
                ssh_disconnect(session);
                ssh_free(session);
            }
        }

        bool open_channel() {
            channel = ssh_channel_new(session);
            if (channel == nullptr) return false;
            return ssh_channel_open_session(channel) == SSH_OK;
        }

        void set_options() {
            ssh_options_set(session, SSH_OPTIONS_HOST, ip);
            ssh_options_set(session, SSH_OPTIONS_USER, user);
            int strict_check = 0;
            ssh_options_set(session, SSH_OPTIONS_STRICTHOSTKEYCHECK, &strict_check);
        }

        void connect() {
            std::cout << "Connecting to " << ip << std::endl;
            if (ssh_connect(session) != SSH_OK) throw std::runtime_error("Error with the connection: " + std::string(ssh_get_error(session)));
        }

        void authenticate() {
            std::cout << "Authenticating" << std::endl;
            if (ssh_userauth_password(session, NULL, password) != SSH_AUTH_SUCCESS) throw std::runtime_error("Error with the authentication: " + std::string(ssh_get_error(session)));
        }

        void execute_command(const char* command) {
            std::cout << "Running: " << command << std::endl;
            if (!open_channel()) throw std::runtime_error("Error opening channel: " + std::string(ssh_get_error(session)));
            if (ssh_channel_request_exec(channel, command) != SSH_OK) throw std::runtime_error("Error executing command: " + std::string(ssh_get_error(session)));
        }
};

void connect_device_ssh(const char* ip, const char* user, const char* password) {
    SSH_manager ssh(ip, user, password);
    const char* command = "sudo docker run -d --rm -p 50051:50051 --name llama-worker alvaropereda/llama-worker";
    ssh.set_options();
    ssh.connect();
    ssh.authenticate();
    ssh.execute_command(command);
}

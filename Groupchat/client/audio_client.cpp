#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>

class AudioClient {
private:
    int socket_fd;
    std::string server_ip;
    int server_port;
    bool connected;
    std::mutex audio_mutex;

public:
    AudioClient(const std::string& ip, int port) 
        : server_ip(ip), server_port(port), connected(false) {}

    bool connect() {
        socket_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (socket_fd == -1) {
            std::cerr << "Failed to create socket" << std::endl;
            return false;
        }

        sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(server_port);
        inet_pton(AF_INET, server_ip.c_str(), &server_addr.sin_addr);

        if (::connect(socket_fd, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            std::cerr << "Connection failed" << std::endl;
            return false;
        }

        connected = true;
        std::cout << "Connected to audio server" << std::endl;
        return true;
    }

    void sendAudio(const std::vector<char>& audio_data) {
        if (!connected) {
            std::cerr << "Not connected to server" << std::endl;
            return;
        }

        std::lock_guard<std::mutex> lock(audio_mutex);
        send(socket_fd, audio_data.data(), audio_data.size(), 0);
    }

    std::vector<char> receiveAudio(size_t buffer_size = 4096) {
        std::vector<char> buffer(buffer_size);
        
        int bytes_received = recv(socket_fd, buffer.data(), buffer_size, 0);
        
        if (bytes_received > 0) {
            buffer.resize(bytes_received);
            return buffer;
        }

        return std::vector<char>();
    }

    void disconnect() {
        if (connected) {
            close(socket_fd);
            connected = false;
            std::cout << "Disconnected from audio server" << std::endl;
        }
    }

    ~AudioClient() {
        disconnect();
    }
};

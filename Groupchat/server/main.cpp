#include "chat_server.h"
#include <iostream>
#include <csignal>
#include <atomic>

std::atomic<bool> running(true);
ChatServer* global_server = nullptr;

void signal_handler(int signal) {
    std::cout << "\nReceived signal " << signal << ", shutting down gracefully...\n";
    running.store(false);
    if (global_server) {
        global_server->shutdown();
    }
    exit(0);
}

int main(int argc, char *argv[]) {
    // Setup signal handlers
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    int port = 8080;
    if (argc >= 2) {
        port = std::stoi(argv[1]);
    }

    size_t numThreads = 4; // could be std::thread::hardware_concurrency()
    ChatServer server(port, numThreads);
    global_server = &server;

    try {
        server.run();
    } catch (const std::exception &ex) {
        std::cerr << "Server error: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}

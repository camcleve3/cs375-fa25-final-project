#include "chat_server.h"
#include <iostream>

int main(int argc, char *argv[]) {
    int port = 8080;
    if (argc >= 2) {
        port = std::stoi(argv[1]);
    }

    size_t numThreads = 4; // could be std::thread::hardware_concurrency()
    ChatServer server(port, numThreads);

    try {
        server.run();
    } catch (const std::exception &ex) {
        std::cerr << "Server error: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}

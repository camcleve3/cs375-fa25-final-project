#include "chat_server.h"

#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

ChatServer::ChatServer(int port, size_t numThreads)
    : port(port), server_fd(-1), pool(numThreads) { }

void ChatServer::setup_socket() {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        std::exit(EXIT_FAILURE);
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = htons(port);

    if (bind(server_fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        std::exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 8) < 0) {
        perror("listen");
        std::exit(EXIT_FAILURE);
    }
    std::cout << "Server listening on port " << port << std::endl;
}

void ChatServer::run() {
    setup_socket();

    while (true) {
        sockaddr_in client_addr{};
        socklen_t addrlen = sizeof(client_addr);
        int clientSocket = accept(server_fd,
                                  (sockaddr*)&client_addr,
                                  &addrlen);
        if (clientSocket < 0) {
            perror("accept");
            continue;
        }
        std::cout << "New client " << clientSocket << " connected\n";

        // Scheduling via thread pool (Round Robin on tasks queue)
        pool.enqueue([this, clientSocket]() {
            handle_client(clientSocket);
        });
    }
}

void ChatServer::handle_client(int clientSocket) {
    bool connected = true;

    // Default group 1 until client sends JOIN
    uint16_t currentGroup = 1;
    groups.joinGroup(clientSocket, currentGroup);

    // Send recent history for group 1
    auto history = groups.cacheManager().getHistory(currentGroup);
    for (auto &msg : history) {
        ChatPacket net = to_network(msg);
        send(clientSocket, &net, sizeof(net), 0);
    }

    while (connected) {
        ChatPacket netPkt{};
        ssize_t bytes = recv(clientSocket, &netPkt, sizeof(netPkt), 0);
        if (bytes <= 0) {
            std::cout << "Client " << clientSocket << " disconnected\n";
            groups.removeClient(clientSocket);
            close(clientSocket);
            connected = false;
            break;
        }

        ChatPacket pkt = to_host(netPkt);

        switch (pkt.type) {
            case MSG_JOIN: {
                uint16_t groupID = pkt.groupID;
                groups.switchGroup(clientSocket, groupID);
                currentGroup = groupID;

                // send cached history
                auto hist = groups.cacheManager().getHistory(groupID);
                for (auto &m : hist) {
                    ChatPacket net = to_network(m);
                    send(clientSocket, &net, sizeof(net), 0);
                }
                break;
            }
            case MSG_SWITCH: {
                uint16_t newGroup = pkt.groupID;
                groups.switchGroup(clientSocket, newGroup);
                currentGroup = newGroup;
                auto hist = groups.cacheManager().getHistory(newGroup);
                for (auto &m : hist) {
                    ChatPacket net = to_network(m);
                    send(clientSocket, &net, sizeof(net), 0);
                }
                break;
            }
            case MSG_TEXT: {
                // Broadcast to all in currentGroup
                groups.broadcast(clientSocket, currentGroup, pkt);
                break;
            }
            default:
                std::cerr << "Unknown packet type from client "
                          << clientSocket << "\n";
                break;
        }
    }
}
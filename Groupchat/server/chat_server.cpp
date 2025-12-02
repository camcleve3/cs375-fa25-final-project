#include "chat_server.h"
#include "shared/metrics.h"
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

        pool.enqueue([this, clientSocket]() {
            handle_client(clientSocket);
        });
    }
}

void ChatServer::handle_client(int clientSocket) {
    groups.joinGroup(clientSocket, 1); // default group
    uint16_t currentGroup = 1;

    // Send recent message history for group 1
    auto history = groups.getGroupHistory(1);
    for (const auto &msg : history) {
        ChatPacket netPkt = to_network(msg);
        send(clientSocket, &netPkt, sizeof(netPkt), 0);
    }

    while (true) {
        ChatPacket netPkt{};
        ssize_t bytes = recv(clientSocket, &netPkt, sizeof(netPkt), 0);

        if (bytes <= 0) {
            std::cout << "Client " << clientSocket << " disconnected\n";
            groups.removeClient(clientSocket);
            close(clientSocket);
            return;
        }

        ChatPacket pkt = to_host(netPkt);
        pkt.senderID = clientSocket; // Set sender ID to socket

        std::cout << "[SERVER] Received type=" << (int)pkt.type
                  << " group=" << pkt.groupID
                  << " from=" << pkt.senderName
                  << " payload=" << pkt.payload << "\n";

        switch (pkt.type) {
            case MSG_JOIN:
            case MSG_SWITCH:
                currentGroup = pkt.groupID;
                groups.switchGroup(clientSocket, currentGroup);
                // Send history for new group
                {
                    auto hist = groups.getGroupHistory(currentGroup);
                    for (const auto &msg : hist) {
                        ChatPacket net = to_network(msg);
                        send(clientSocket, &net, sizeof(net), 0);
                    }
                }
                break;

            case MSG_TEXT:
                groups.broadcast(clientSocket, currentGroup, pkt);
                PerformanceMetrics::getInstance().incrementMessageCount();
                break;

            case MSG_LIST_GROUPS:
                {
                    auto activeGroups = groups.getActiveGroups();
                    std::string groupList;
                    for (size_t i = 0; i < activeGroups.size(); ++i) {
                        groupList += std::to_string(activeGroups[i]);
                        if (i < activeGroups.size() - 1) groupList += ", ";
                    }
                    ChatPacket resp = make_packet(MSG_LIST_GROUPS, 0, groupList, 0, "SERVER");
                    ChatPacket net = to_network(resp);
                    send(clientSocket, &net, sizeof(net), 0);
                }
                break;

            default:
                std::cout << "Unknown packet type\n";
        }
    }
}

void ChatServer::shutdown() {
    std::cout << "Shutting down server...\n";
    
    // Log final performance metrics
    PerformanceMetrics::getInstance().logMetrics();
    
    if (server_fd != -1) {
        close(server_fd);
    }
    std::cout << "Server shutdown complete.\n";
}

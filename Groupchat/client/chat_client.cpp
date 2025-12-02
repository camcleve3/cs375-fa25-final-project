#include "chat_client.h"
#include <iostream>
#include <thread>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

ChatClient::ChatClient(const std::string &host, int port)
    : host(host), port(port), sock(-1) { }

void ChatClient::connect_to_server() {
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        std::exit(1);
    }

    sockaddr_in serv{};
    serv.sin_family = AF_INET;
    serv.sin_port   = htons(port);
    inet_pton(AF_INET, host.c_str(), &serv.sin_addr);

    if (connect(sock, (sockaddr*)&serv, sizeof(serv)) < 0) {
        perror("connect");
        std::exit(1);
    }

    std::cout << "Connected to server\n";
}

void ChatClient::send_loop() {
    uint16_t currentGroup = 1;

    // Get username
    std::cout << "Enter your username: ";
    std::getline(std::cin, username);

    // Auto-join group 1
    ChatPacket joinPkt = make_packet(MSG_JOIN, currentGroup, "", 0, username);
    ChatPacket netPkt = to_network(joinPkt);
    send(sock, &netPkt, sizeof(netPkt), 0);

    while (true) {
        std::string line;
        std::getline(std::cin, line);

        if (line.rfind("/switch ", 0) == 0) {
            int g = std::stoi(line.substr(8));
            currentGroup = g;
            ChatPacket p = make_packet(MSG_SWITCH, g, "", 0, username);
            ChatPacket n = to_network(p);
            send(sock, &n, sizeof(n), 0);
            std::cout << "Switched to group " << g << "\n";
            continue;
        }

        if (line == "/quit") {
            close(sock);
            exit(0);
        }

        if (line.rfind("/list", 0) == 0) {
            ChatPacket p = make_packet(MSG_LIST_GROUPS, 0, "", 0, username);
            ChatPacket n = to_network(p);
            send(sock, &n, sizeof(n), 0);
            continue;
        }

        ChatPacket pkt = make_packet(MSG_TEXT, currentGroup, line, 0, username);
        ChatPacket net = to_network(pkt);
        send(sock, &net, sizeof(net), 0);

        std::cout << "[You] " << line << "\n";
    }
}

void ChatClient::receive_loop() {
    while (true) {
        ChatPacket net{};
        ssize_t bytes = recv(sock, &net, sizeof(net), 0);
        if (bytes <= 0) {
            std::cout << "Disconnected\n";
            exit(0);
        }

        ChatPacket pkt = to_host(net);
        if (pkt.type == MSG_TEXT) {
            std::cout << "[G" << pkt.groupID << "][" << pkt.senderName << "] "
                      << pkt.payload << "\n";
        } else if (pkt.type == MSG_LIST_GROUPS) {
            std::cout << "Active groups: " << pkt.payload << "\n";
        }
    }
}

void ChatClient::run() {
    connect_to_server();
    std::thread recvThread(&ChatClient::receive_loop, this);
    send_loop();
    recvThread.join();
}

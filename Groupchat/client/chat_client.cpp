#include "chat_client.h"

#include <iostream>
#include <thread>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

ChatClient::ChatClient(const std::string &host, int port)
    : host(host), port(port), sock(-1) {}

void ChatClient::connect_to_server() {
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        std::exit(EXIT_FAILURE);
    }

    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port   = htons(port);

    if (inet_pton(AF_INET, host.c_str(), &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address\n";
        std::exit(EXIT_FAILURE);
    }

    if (connect(sock, (sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connect");
        std::exit(EXIT_FAILURE);
    }
    std::cout << "Connected to server " << host << ":" << port << "\n";
}

void ChatClient::send_loop() {
    std::cout << "Enter /join <id>, /switch <id>, or just message text.\n";
    uint16_t currentGroup = 1;

    // Join default group 1
    ChatPacket joinPkt = make_packet(MSG_JOIN, currentGroup, "join");
    ChatPacket netJoin = to_network(joinPkt);
    send(sock, &netJoin, sizeof(netJoin), 0);

    while (true) {
        std::string line;
        if (!std::getline(std::cin, line)) break;

        if (line.rfind("/join ", 0) == 0 || line.rfind("/switch ", 0) == 0) {
            int id = std::stoi(line.substr(line.find(' ') + 1));
            currentGroup = static_cast<uint16_t>(id);
            MessageType type = (line[1] == 'j') ? MSG_JOIN : MSG_SWITCH;
            ChatPacket pkt = make_packet(type, currentGroup, "");
            ChatPacket net = to_network(pkt);
            send(sock, &net, sizeof(net), 0);
            continue;
        }

        if (line == "/quit") {
            close(sock);
            std::exit(0);
        }

        ChatPacket msg = make_packet(MSG_TEXT, currentGroup, line);
        ChatPacket net = to_network(msg);
        send(sock, &net, sizeof(net), 0);
    }
}

void ChatClient::receive_loop() {
    while (true) {
        ChatPacket netPkt{};
        ssize_t bytes = recv(sock, &netPkt, sizeof(netPkt), 0);
        if (bytes <= 0) {
            std::cout << "Disconnected from server.\n";
            std::exit(0);
        }
        ChatPacket pkt = to_host(netPkt);
        if (pkt.type == MSG_TEXT) {
            std::cout << "[G" << pkt.groupID << "] "
                      << pkt.payload << "\n";
        }
    }
}

void ChatClient::run() {
    connect_to_server();
    std::thread recvThread(&ChatClient::receive_loop, this);
    send_loop();
    recvThread.join();
}

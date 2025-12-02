// tests/bot_test.cpp
#include "shared/protocol.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <chrono>

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

    connect(sock, (sockaddr*)&addr, sizeof(addr));

    ChatPacket joinPkt = make_packet(MSG_JOIN, 1, "");
    ChatPacket netJoin = to_network(joinPkt);
    send(sock, &netJoin, sizeof(netJoin), 0);

    using namespace std::chrono;
    auto start = high_resolution_clock::now();

    for (int i = 0; i < 100; ++i) {
        ChatPacket pkt = make_packet(MSG_TEXT, 1, "bot message " + std::to_string(i));
        ChatPacket net = to_network(pkt);
        send(sock, &net, sizeof(net), 0);
    }

    auto end = high_resolution_clock::now();
    auto ms = duration_cast<milliseconds>(end - start).count();
    std::cout << "Sent 100 messages in " << ms << " ms\n";

    close(sock);
    return 0;
}

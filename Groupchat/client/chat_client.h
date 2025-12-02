
#pragma once

#include "shared/protocol.h"
#include <string>

class ChatClient {
public:
    ChatClient(const std::string &host, int port);

    void run();

private:
    std::string host;
    int port;
    int sock;

    void connect_to_server();
    void send_loop();
    void receive_loop();
};

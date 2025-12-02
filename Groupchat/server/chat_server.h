#pragma once

#include "thread_pool.h"
#include "group_manager.h"
#include "shared/protocol.h"

class ChatServer {
public:
    ChatServer(int port, size_t numThreads);

    void run();

private:
    int port;
    int server_fd;
    ThreadPool pool;
    GroupManager groups;

    void setup_socket();
    void handle_client(int clientSocket);
};

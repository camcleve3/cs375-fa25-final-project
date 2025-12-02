#pragma once

#include "thread_pool.h"
#include "group_manager.h"
#include "shared/protocol.h"
#include "shared/virtual_memory.h"

class ChatServer {
public:
    ChatServer(int port, size_t numThreads);

    void run();
    void shutdown();

private:
    int port;
    int server_fd;
    ThreadPool pool;
    GroupManager groups;
    VirtualMemory vmem;  // Virtual memory simulator

    void setup_socket();
    void handle_client(int clientSocket);
};

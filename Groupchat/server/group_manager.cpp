// server/group_manager.cpp
#include "group_manager.h"
#include <sys/socket.h>
#include <unistd.h>
#include <algorithm>
#include <fstream>
#include <iostream>

GroupManager::GroupManager()
    : cache(20) {}

void GroupManager::joinGroup(int clientSocket, uint16_t groupID) {
    std::lock_guard<std::mutex> lock(mtx);
    groupMembers[groupID].push_back(clientSocket);
    clientGroup[clientSocket] = groupID;
}

void GroupManager::switchGroup(int clientSocket, uint16_t newGroupID) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = clientGroup.find(clientSocket);
    if (it != clientGroup.end()) {
        uint16_t oldGroup = it->second;
        auto &vec = groupMembers[oldGroup];
        vec.erase(std::remove(vec.begin(), vec.end(), clientSocket),
                  vec.end());
    }
    groupMembers[newGroupID].push_back(clientSocket);
    clientGroup[clientSocket] = newGroupID;
}

void GroupManager::removeClient(int clientSocket) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = clientGroup.find(clientSocket);
    if (it != clientGroup.end()) {
        uint16_t groupID = it->second;
        auto &vec = groupMembers[groupID];
        vec.erase(std::remove(vec.begin(), vec.end(), clientSocket),
                  vec.end());
        clientGroup.erase(it);
    }
}

void GroupManager::broadcast(int senderSocket,
                             uint16_t groupID,
                             const ChatPacket &pktHost) {
    // Save to cache and log first
    cache.addMessage(groupID, pktHost);

    // Log to file
    {
        std::ofstream log("logs/chat_log.txt", std::ios::app);
        log << pktHost.timestamp << " | group " << groupID
            << " | msg: " << pktHost.payload << "\n";
    }

    ChatPacket netPkt = to_network(pktHost);

    std::lock_guard<std::mutex> lock(mtx);
    auto it = groupMembers.find(groupID);
    if (it == groupMembers.end()) return;

    for (int sock : it->second) {
        // Optionally skip sender for echo
        if (sock == senderSocket) continue;
        ssize_t sent = send(sock, &netPkt, sizeof(netPkt), 0);
        if (sent <= 0) {
            std::cerr << "send failed to client " << sock << "\n";
        }
    }
}

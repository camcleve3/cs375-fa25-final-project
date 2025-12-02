// server/group_manager.h
#pragma once

#include "shared/protocol.h"
#include "shared/cache.h"
#include <unordered_map>
#include <vector>
#include <mutex>

class GroupManager {
public:
    GroupManager();

    void joinGroup(int clientSocket, uint16_t groupID);
    void switchGroup(int clientSocket, uint16_t newGroupID);
    void removeClient(int clientSocket);

    void broadcast(int senderSocket,
                   uint16_t groupID,
                   const ChatPacket &pkt);

    std::vector<uint16_t> getActiveGroups();
    std::vector<ChatPacket> getGroupHistory(uint16_t groupID);

    GroupCacheManager &cacheManager() { return cache; }

private:
    std::mutex mtx;
    // groupID -> list of client sockets
    std::unordered_map<uint16_t, std::vector<int>> groupMembers;
    // client socket -> groupID
    std::unordered_map<int, uint16_t> clientGroup;

    GroupCacheManager cache;
};


// shared/cache.h
#pragma once

#include "protocol.h"
#include <vector>
#include <mutex>
#include <unordered_map>

class CircularCache {
public:
    explicit CircularCache(size_t capacity = 20);

    void add(const ChatPacket &pkt);
    std::vector<ChatPacket> getAll() const;

private:
    size_t capacity;
    size_t head;
    size_t count;
    std::vector<ChatPacket> buffer;
};

class GroupCacheManager {
public:
    GroupCacheManager(size_t capacityPerGroup = 20);

    void addMessage(uint16_t groupID, const ChatPacket &pkt);
    std::vector<ChatPacket> getHistory(uint16_t groupID);

private:
    mutable std::mutex mtx;
    size_t perGroupCapacity;
    std::unordered_map<uint16_t, CircularCache> caches;
};


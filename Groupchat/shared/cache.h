// shared/cache.h
#pragma once

#include "protocol.h"
#include <vector>
#include <mutex>
#include <unordered_map>
#include <chrono>

struct CachedMessage {
    ChatPacket packet;
    std::chrono::system_clock::time_point timestamp;
};

class CircularCache {
public:
    explicit CircularCache(size_t capacity = 20, uint32_t ttlSeconds = 300);

    void add(const ChatPacket &pkt);
    std::vector<ChatPacket> getAll() const;
    void evictExpired();

private:
    size_t capacity;
    uint32_t ttl;  // Time to live in seconds
    size_t head;
    size_t count;
    std::vector<CachedMessage> buffer;
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


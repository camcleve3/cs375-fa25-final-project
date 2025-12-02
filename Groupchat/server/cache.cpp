// shared/cache.cpp
#include "shared/cache.h"

// -------- CircularCache --------

CircularCache::CircularCache(size_t cap)
    : capacity(cap),
      head(0),
      count(0),
      buffer(cap) {}

void CircularCache::add(const ChatPacket &pkt) {
    buffer[head] = pkt;
    head = (head + 1) % capacity;
    if (count < capacity) count++;
}

std::vector<ChatPacket> CircularCache::getAll() const {
    std::vector<ChatPacket> out;
    out.reserve(count);
    // Oldest message is at (head - count + capacity) % capacity
    size_t start = (head + capacity - count) % capacity;
    for (size_t i = 0; i < count; ++i) {
        out.push_back(buffer[(start + i) % capacity]);
    }
    return out;
}

// -------- GroupCacheManager --------

GroupCacheManager::GroupCacheManager(size_t capacityPerGroup)
    : perGroupCapacity(capacityPerGroup) {}

void GroupCacheManager::addMessage(uint16_t groupID,
                                   const ChatPacket &pkt) {
    std::lock_guard<std::mutex> lock(mtx);
    auto &cache = caches[groupID];
    if (cache.getAll().empty()) {
        cache = CircularCache(perGroupCapacity);
    }
    cache.add(pkt);
}

std::vector<ChatPacket> GroupCacheManager::getHistory(uint16_t groupID) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = caches.find(groupID);
    if (it == caches.end()) return {};
    return it->second.getAll();
}

#include "cache.h"
#include "metrics.h"

CircularCache::CircularCache(size_t capacity, uint32_t ttlSeconds)
    : capacity(capacity), ttl(ttlSeconds), head(0), count(0),
      buffer(capacity) { }

void CircularCache::add(const ChatPacket &pkt) {
    CachedMessage msg;
    msg.packet = pkt;
    msg.timestamp = std::chrono::system_clock::now();
    
    buffer[head] = msg;
    head = (head + 1) % capacity;
    if (count < capacity) count++;
}

void CircularCache::evictExpired() {
    auto now = std::chrono::system_clock::now();
    size_t newCount = 0;
    
    for (size_t i = 0; i < count; ++i) {
        size_t idx = (head + capacity - count + i) % capacity;
        auto age = std::chrono::duration_cast<std::chrono::seconds>(
            now - buffer[idx].timestamp).count();
        
        if (age < ttl) {
            ++newCount;
        }
    }
    
    count = newCount;
}

std::vector<ChatPacket> CircularCache::getAll() const {
    std::vector<ChatPacket> out;
    out.reserve(count);

    auto now = std::chrono::system_clock::now();
    size_t start = (head + capacity - count) % capacity;
    
    for (size_t i = 0; i < count; ++i) {
        size_t idx = (start + i) % capacity;
        auto age = std::chrono::duration_cast<std::chrono::seconds>(
            now - buffer[idx].timestamp).count();
        
        if (age < ttl) {
            out.push_back(buffer[idx].packet);
            PerformanceMetrics::getInstance().incrementCacheHit();
        } else {
            PerformanceMetrics::getInstance().incrementCacheMiss();
        }
    }

    return out;
}

GroupCacheManager::GroupCacheManager(size_t per)
    : perGroupCapacity(per) { }

void GroupCacheManager::addMessage(uint16_t groupID,
                                   const ChatPacket &pkt) {
    std::lock_guard<std::mutex> lock(mtx);
    auto &cache = caches[groupID];
    cache.add(pkt);
}

std::vector<ChatPacket> GroupCacheManager::getHistory(uint16_t groupID) {
    std::lock_guard<std::mutex> lock(mtx);
    caches[groupID].evictExpired();
    return caches[groupID].getAll();
}

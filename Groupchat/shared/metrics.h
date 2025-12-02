// shared/metrics.h
#pragma once

#include <chrono>
#include <atomic>
#include <mutex>
#include <fstream>
#include <iostream>

class PerformanceMetrics {
public:
    static PerformanceMetrics& getInstance() {
        static PerformanceMetrics instance;
        return instance;
    }

    void incrementMessageCount() {
        messageCount.fetch_add(1);
    }

    void incrementCacheHit() {
        cacheHits.fetch_add(1);
    }

    void incrementCacheMiss() {
        cacheMisses.fetch_add(1);
    }

    void recordThreadUsage(size_t active) {
        std::lock_guard<std::mutex> lock(mtx);
        activeThreads = active;
    }

    void recordPageFault() {
        pageFaults.fetch_add(1);
    }

    void logMetrics() {
        std::lock_guard<std::mutex> lock(mtx);
        
        auto now = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(
            now - startTime).count();
        
        double msgRate = duration > 0 ? (double)messageCount.load() / duration : 0;
        double cacheHitRate = 0;
        size_t totalCache = cacheHits.load() + cacheMisses.load();
        if (totalCache > 0) {
            cacheHitRate = (double)cacheHits.load() / totalCache * 100.0;
        }

        std::ofstream log("../Groupchat/logs/performance.txt", std::ios::app);
        log << "=== Performance Metrics ===\n";
        log << "Uptime: " << duration << " seconds\n";
        log << "Total Messages: " << messageCount.load() << "\n";
        log << "Message Rate: " << msgRate << " msg/sec\n";
        log << "Cache Hits: " << cacheHits.load() << "\n";
        log << "Cache Misses: " << cacheMisses.load() << "\n";
        log << "Cache Hit Rate: " << cacheHitRate << "%\n";
        log << "Active Threads: " << activeThreads << "\n";
        log << "Page Faults: " << pageFaults.load() << "\n";
        log << "===========================\n\n";
        log.close();

        std::cout << "Performance metrics logged\n";
    }

private:
    PerformanceMetrics() : startTime(std::chrono::system_clock::now()),
                           activeThreads(0) {}

    std::chrono::system_clock::time_point startTime;
    std::atomic<size_t> messageCount{0};
    std::atomic<size_t> cacheHits{0};
    std::atomic<size_t> cacheMisses{0};
    std::atomic<size_t> pageFaults{0};
    size_t activeThreads;
    std::mutex mtx;
};

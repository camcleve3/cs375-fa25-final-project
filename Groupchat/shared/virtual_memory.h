// shared/virtual_memory.h
#pragma once

#include <vector>
#include <unordered_map>
#include <mutex>
#include <cstdint>
#include <cstring>
#include "metrics.h"

// Virtual memory simulator with paging
class VirtualMemory {
public:
    static constexpr size_t PAGE_SIZE = 256;        // bytes per page
    static constexpr size_t NUM_PAGES = 16;          // total physical pages
    static constexpr size_t MEMORY_SIZE = PAGE_SIZE * NUM_PAGES;

    VirtualMemory() : nextPageId(0) {
        physicalMemory.resize(MEMORY_SIZE, 0);
        pageTable.resize(NUM_PAGES, -1);  // -1 = empty
    }

    // Allocate virtual memory (returns page ID)
    int allocate(size_t size) {
        std::lock_guard<std::mutex> lock(mtx);
        
        size_t pagesNeeded = (size + PAGE_SIZE - 1) / PAGE_SIZE;
        
        // Find free pages
        std::vector<int> freePages;
        for (size_t i = 0; i < NUM_PAGES; ++i) {
            if (pageTable[i] == -1) {
                freePages.push_back(i);
            }
            if (freePages.size() >= pagesNeeded) break;
        }

        if (freePages.size() < pagesNeeded) {
            // Page fault - need to evict (use LRU-like strategy)
            PerformanceMetrics::getInstance().recordPageFault();
            evictPages(pagesNeeded - freePages.size());
            
            // Re-find free pages
            freePages.clear();
            for (size_t i = 0; i < NUM_PAGES; ++i) {
                if (pageTable[i] == -1) {
                    freePages.push_back(i);
                }
                if (freePages.size() >= pagesNeeded) break;
            }
        }

        int virtualPageId = nextPageId++;
        
        // Map virtual pages to physical pages
        for (size_t i = 0; i < pagesNeeded; ++i) {
            int physicalPage = freePages[i];
            pageTable[physicalPage] = virtualPageId;
            virtualToPhysical[virtualPageId].push_back(physicalPage);
        }

        return virtualPageId;
    }

    // Write data to virtual page
    bool write(int virtualPageId, const void* data, size_t size) {
        std::lock_guard<std::mutex> lock(mtx);
        
        auto it = virtualToPhysical.find(virtualPageId);
        if (it == virtualToPhysical.end()) return false;

        size_t offset = 0;
        for (int physPage : it->second) {
            size_t toWrite = std::min(PAGE_SIZE, size - offset);
            std::memcpy(&physicalMemory[physPage * PAGE_SIZE],
                       static_cast<const char*>(data) + offset,
                       toWrite);
            offset += toWrite;
            if (offset >= size) break;
        }

        return true;
    }

    // Read data from virtual page
    bool read(int virtualPageId, void* data, size_t size) {
        std::lock_guard<std::mutex> lock(mtx);
        
        auto it = virtualToPhysical.find(virtualPageId);
        if (it == virtualToPhysical.end()) return false;

        size_t offset = 0;
        for (int physPage : it->second) {
            size_t toRead = std::min(PAGE_SIZE, size - offset);
            std::memcpy(static_cast<char*>(data) + offset,
                       &physicalMemory[physPage * PAGE_SIZE],
                       toRead);
            offset += toRead;
            if (offset >= size) break;
        }

        return true;
    }

    // Free virtual page
    void deallocate(int virtualPageId) {
        std::lock_guard<std::mutex> lock(mtx);
        
        auto it = virtualToPhysical.find(virtualPageId);
        if (it == virtualToPhysical.end()) return;

        // Free physical pages
        for (int physPage : it->second) {
            pageTable[physPage] = -1;
        }

        virtualToPhysical.erase(it);
    }

    size_t getUsedPages() const {
        std::lock_guard<std::mutex> lock(mtx);
        size_t used = 0;
        for (int page : pageTable) {
            if (page != -1) ++used;
        }
        return used;
    }

private:
    void evictPages(size_t count) {
        // Simple FIFO eviction (could be enhanced with LRU)
        size_t evicted = 0;
        for (size_t i = 0; i < NUM_PAGES && evicted < count; ++i) {
            if (pageTable[i] != -1) {
                int virtualId = pageTable[i];
                pageTable[i] = -1;
                
                // Remove from mapping
                auto it = virtualToPhysical.find(virtualId);
                if (it != virtualToPhysical.end()) {
                    auto &pages = it->second;
                    pages.erase(std::remove(pages.begin(), pages.end(), i), pages.end());
                    if (pages.empty()) {
                        virtualToPhysical.erase(it);
                    }
                }
                ++evicted;
            }
        }
    }

    mutable std::mutex mtx;
    std::vector<uint8_t> physicalMemory;
    std::vector<int> pageTable;  // Physical page -> virtual page ID (-1 if free)
    std::unordered_map<int, std::vector<int>> virtualToPhysical;  // Virtual ID -> physical pages
    int nextPageId;
};

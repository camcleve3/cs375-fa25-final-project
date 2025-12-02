// server/thread_pool.h
#pragma once

#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

struct Task {
    std::function<void()> func;
    int priority;  // Lower number = higher priority (for SJF simulation)
    
    bool operator<(const Task &other) const {
        return priority > other.priority;  // Min-heap
    }
};

class ThreadPool {
public:
    explicit ThreadPool(size_t threads);
    ~ThreadPool();

    void enqueue(std::function<void()> task, int priority = 5);

    size_t size() const { return workers.size(); }

private:
    std::vector<std::thread> workers;
    std::priority_queue<Task> tasks;  // Priority queue for SJF scheduling

    mutable std::mutex queue_mutex;
    std::condition_variable condition;
    std::atomic<bool> stop;
};


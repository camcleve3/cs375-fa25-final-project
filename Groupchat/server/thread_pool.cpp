#include "thread_pool.h"

ThreadPool::ThreadPool(size_t threads) : stop(false) {
    for (size_t i = 0; i < threads; ++i) {
        workers.emplace_back([this]() {
            while (true) {
                Task task;

                {
                    std::unique_lock<std::mutex> lock(queue_mutex);
                    condition.wait(lock, [this]() {
                        return stop.load() || !tasks.empty();
                    });

                    if (stop.load() && tasks.empty())
                        return;

                    task = tasks.top();
                    tasks.pop();
                }

                task.func();
            }
        });
    }
}

ThreadPool::~ThreadPool() {
    stop.store(true);
    condition.notify_all();
    for (auto &worker : workers)
        worker.join();
}

void ThreadPool::enqueue(std::function<void()> f, int priority) {
    {
        std::lock_guard<std::mutex> lock(queue_mutex);
        tasks.push({std::move(f), priority});
    }
    condition.notify_one();
}

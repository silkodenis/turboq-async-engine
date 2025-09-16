#pragma once

#include <functional>
#include <chrono>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>

namespace turboq {

class Queue;

class Timer {
public:
    using Task = std::function<void()>;

    static Timer& instance();

    void schedule(Task task,
                  std::chrono::steady_clock::time_point when,
                  Queue& queue);

private:
    struct ScheduledTask {
        std::chrono::steady_clock::time_point when;
        Task task;
        Queue* queue;

        bool operator>(const ScheduledTask& other) const {
            return when > other.when;
        }
    };

    std::priority_queue<ScheduledTask,
                        std::vector<ScheduledTask>,
                        std::greater<>> tasks_;

    std::mutex mutex_;
    std::condition_variable cv_;
    bool stop_ = false;
    std::thread worker_;

    Timer();
    ~Timer();

    void run();
};

} 

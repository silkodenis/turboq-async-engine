#pragma once

#include <functional>
#include <queue>
#include <mutex>
#include <thread>
#include <vector>
#include <condition_variable>
#include <iostream>

namespace turboq {

class ThreadPool {
public:
    enum class QoS {
        UserInteractive = 3,
        UserInitiated   = 2,
        Utility         = 1,
        Background      = 0   
    };

    using Task = std::function<void()>;

    static ThreadPool& instance(size_t threads = std::thread::hardware_concurrency());
    explicit ThreadPool(size_t threads = std::thread::hardware_concurrency());
    ~ThreadPool();

    void submit(Task task, QoS qos = QoS::Utility);

private:
    struct PrioritizedTask {
        Task task;
        QoS qos;

        PrioritizedTask(Task t, QoS q);

        bool operator<(const PrioritizedTask& other) const {
            return static_cast<int>(qos) < static_cast<int>(other.qos);
        }
    };

    std::vector<std::thread> workers_;
    std::priority_queue<PrioritizedTask> tasks_;
    std::mutex mutex_;
    std::condition_variable cv_;
    bool stop_;

    void worker_loop();
};

} 

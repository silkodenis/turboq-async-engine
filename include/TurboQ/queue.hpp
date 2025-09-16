#pragma once

#include <TurboQ/thread_pool.hpp>
#include <TurboQ/timer.hpp>

#include <queue>
#include <mutex>
#include <string>
#include <functional>
#include <chrono>
#include <thread>
#include <iostream>


namespace turboq {

class Queue {
public:
    using Task = std::function<void()>;

    enum class Type { Serial, Concurrent };

    Queue(std::string name,
          Type type = Type::Serial,
          ThreadPool::QoS qos = ThreadPool::QoS::Utility);

    static Queue& global(ThreadPool::QoS qos = ThreadPool::QoS::Utility);

    void async(Task task);
    void async_at(std::chrono::steady_clock::time_point when, Task task);
    void async_after(std::chrono::milliseconds delay, Task task);
    void sync(Task task);

private:
    void submit_next();

    std::string name_;
    Type type_;
    ThreadPool::QoS qos_;

    std::mutex mutex_;
    std::queue<Task> tasks_;
    bool isRunning_ = false;

    std::thread::id running_thread_id_;
};

inline Queue& global(ThreadPool::QoS qos = ThreadPool::QoS::Utility) {
    return Queue::global(qos);
}

} 


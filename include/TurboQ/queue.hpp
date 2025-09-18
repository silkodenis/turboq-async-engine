/*
 * Copyright 2025 Denis Silko
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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


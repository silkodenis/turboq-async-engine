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
    std::thread worker_;
    bool stop_;

    Timer();
    ~Timer();

    void run();
};

} 

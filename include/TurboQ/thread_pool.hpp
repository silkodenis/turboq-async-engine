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
#include <queue>
#include <mutex>
#include <thread>
#include <vector>
#include <condition_variable>
#include <iostream>

namespace turboq {

/**
 * @brief A thread pool that executes tasks with different priorities (QoS).
 *
 * ThreadPool allows submitting tasks for concurrent execution using a pool of worker threads.
 * Tasks can be prioritized according to Quality of Service (QoS).
 */
class ThreadPool {
public:
    /**
     * @brief Defines task priority levels (Quality of Service).
     */
    enum class QoS {
        UserInteractive = 3, ///< Highest priority, e.g., UI tasks.
        UserInitiated   = 2, ///< High priority, tasks initiated by the user.
        Utility         = 1, ///< Medium priority, background computations.
        Background      = 0  ///< Lowest priority, long-running background tasks.
    };

    using Task = std::function<void()>; ///< Represents a task to be executed.

    /**
     * @brief Returns a singleton instance of ThreadPool.
     *
     * @param threads Number of worker threads. Default is the number of hardware cores.
     * @return Reference to the ThreadPool instance.
     */
    static ThreadPool& instance(size_t threads = std::thread::hardware_concurrency());

    /**
     * @brief Constructs a ThreadPool with a specified number of threads.
     *
     * @param threads Number of worker threads. Default is the number of hardware cores.
     */
    explicit ThreadPool(size_t threads = std::thread::hardware_concurrency());

    /**
     * @brief Destroys the ThreadPool and joins all worker threads.
     */
    ~ThreadPool();

    /**
     * @brief Submits a task to the thread pool for execution.
     *
     * @param task The task to execute.
     * @param qos Quality of Service for task priority. Default is Utility.
     */
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

} // namespace turboq

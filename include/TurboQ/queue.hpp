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
#include <atomic>
#include <functional>
#include <chrono>
#include <thread>
#include <iostream>

namespace turboq {

/**
 * @brief Represents a task queue that can execute tasks serially or concurrently.
 *
 * Queue allows scheduling tasks asynchronously, synchronously, or at a specific time.
 * It uses a ThreadPool internally to execute tasks according to the specified QoS.
 *
 * @note A Queue instance must remain alive as long as there are
 *       delayed or scheduled tasks associated with it.
 */
class Queue {
public:
    using Task = std::function<void()>;

    /**
     * @brief Defines the type of execution for the queue.
     */
    enum class Type {
        Serial,     ///< Tasks are executed one by one in order.
        Concurrent  ///< Tasks may be executed concurrently.
    };

    /**
     * @brief Constructs a new Queue.
     *
     * @param name Name of the queue, useful for debugging.
     * @param type Type of execution (Serial or Concurrent). Default is Serial.
     * @param qos Quality of Service for thread pool execution. Default is Utility.
     */
    Queue(std::string name = generate_name(),
          Type type = Type::Serial,
          ThreadPool::QoS qos = ThreadPool::QoS::Utility);

    /**
     * @brief Returns a global shared queue with the specified QoS.
     *
     * @param qos Quality of Service for the global queue. Default is Utility.
     * @return Reference to a global Queue instance.
     */
    static Queue& global(ThreadPool::QoS qos = ThreadPool::QoS::Utility);

    /**
     * @brief Submits a task for asynchronous execution.
     *
     * @param task The task to execute.
     */
    void async(Task task);

    /**
     * @brief Schedules a task to execute at a specific time point.
     *
     * @warning The queue object must outlive the scheduled task.
     * If the queue is destroyed before the scheduled time,
     * behavior is undefined (likely crash).
     *
     * @param when Time point when the task should run.
     * @param task The task to execute.
     */
    void async_at(std::chrono::steady_clock::time_point when, Task task);

    /**
     * @brief Schedules a task to execute after a specified delay.
     *
     * @warning The queue object must outlive the scheduled task.
     * If the queue is destroyed before the task is executed,
     * behavior is undefined (likely crash).
     *
     * @param delay Duration to wait before executing the task.
     * @param task The task to execute.
     */
    void async_after(std::chrono::milliseconds delay, Task task);

    /**
     * @brief Executes a task synchronously.
     *
     * If called from the queue’s thread, the task is executed immediately.
     * Otherwise, it waits until the task completes.
     *
     * @param task The task to execute.
     */
    void sync(Task task);

private:
    static std::string generate_name() {
        static std::atomic<int> counter{0};
        return "queue_" + std::to_string(counter++);
    }
    
    void submit_next();

    std::string name_;
    Type type_;
    ThreadPool::QoS qos_;

    std::mutex mutex_;
    std::queue<Task> tasks_;
    bool is_running_;

    std::thread::id running_thread_id_;
};

/**
 * @brief Returns a global queue with the specified QoS.
 *
 * Convenience function wrapping Queue::global().
 *
 * @param qos Quality of Service for the global queue. Default is Utility.
 * @return Reference to the global Queue instance.
 */
inline Queue& global(ThreadPool::QoS qos = ThreadPool::QoS::Utility) {
    return Queue::global(qos);
}

} // namespace turboq


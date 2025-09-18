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

#include <TurboQ/thread_pool.hpp>

namespace turboq {

ThreadPool& ThreadPool::instance(size_t threads) {
    static ThreadPool pool(threads);
    return pool;
}

ThreadPool::ThreadPool(size_t threads) : stop_(false) {
    for (size_t i = 0; i < threads; i++) {
        workers_.emplace_back([this] { this->worker_loop(); });
    }
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(mutex_);
        stop_ = true;
        cv_.notify_all();
    }
    for (auto& t : workers_) {
        if (t.joinable())
            t.join();
    }
}

ThreadPool::PrioritizedTask::PrioritizedTask(Task t, QoS q)
    : task(std::move(t)), qos(q) {}

void ThreadPool::submit(Task task, QoS qos) {
    {
        std::unique_lock<std::mutex> lock(mutex_);
        tasks_.emplace(std::move(task), qos);
    }
    cv_.notify_one();
}

void ThreadPool::worker_loop() {
    while (true) {
        PrioritizedTask task([] {}, QoS::Background);

        {
            std::unique_lock<std::mutex> lock(mutex_);
            cv_.wait(lock, [this] { return stop_ || !tasks_.empty(); });

            if (stop_ && tasks_.empty())
                return;

            task = std::move(const_cast<PrioritizedTask&>(tasks_.top()));
            tasks_.pop();
        }

        try {
            task.task();
        } catch (const std::exception& e) {
            std::cerr << "Task exception: " << e.what() << "\n";
        } catch (...) {
            std::cerr << "Task exception: unknown\n";
        }
    }
}

}

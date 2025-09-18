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

#include <TurboQ/queue.hpp>
#include <assert.h>

namespace turboq {

Queue::Queue(std::string name,
             Type type,
             ThreadPool::QoS qos)
    : name_(std::move(name)), type_(type), qos_(qos) {}

Queue& Queue::global(ThreadPool::QoS qos) {
    static Queue ui("global_ui", Queue::Type::Concurrent, ThreadPool::QoS::UserInteractive);
    static Queue initiated("global_initiated", Queue::Type::Concurrent, ThreadPool::QoS::UserInitiated);
    static Queue utility("global_utility", Queue::Type::Concurrent, ThreadPool::QoS::Utility);
    static Queue background("global_background", Queue::Type::Concurrent, ThreadPool::QoS::Background);

    switch (qos) {
        case ThreadPool::QoS::UserInteractive: return ui;
        case ThreadPool::QoS::UserInitiated:   return initiated;
        case ThreadPool::QoS::Utility:         return utility;
        case ThreadPool::QoS::Background:      return background;
    }
    return utility;
}

void Queue::async(Task task) {
    if (type_ == Type::Concurrent) {
        ThreadPool::instance().submit(std::move(task), qos_);
    } else {
        std::unique_lock<std::mutex> lock(mutex_);
        tasks_.push(std::move(task));
        if (!isRunning_) {
            isRunning_ = true;
            submit_next();
        }
    }
}

void Queue::async_at(std::chrono::steady_clock::time_point when, Task task) {
    Timer::instance().schedule(std::move(task), when, *this);
}

void Queue::async_after(std::chrono::milliseconds delay, Task task) {
    auto when = std::chrono::steady_clock::now() + delay;
    Timer::instance().schedule(std::move(task), when, *this);
}

/*
void Queue::sync(Task task) {
    std::mutex m;
    std::condition_variable cv;
    bool done = false;

    auto wrapper = [&] {
        try {
            task();
        } catch (...) {
            std::cerr << "Queue[" << name_ << "] exception in sync\n";
        }
        {
            std::lock_guard<std::mutex> lock(m);
            done = true;
        }
        cv.notify_one();
    };

    if (type_ == Type::Concurrent) {
        ThreadPool::instance().submit(wrapper, qos_);
    } else {
        if (std::this_thread::get_id() == running_thread_id_) {
            assert(false && "Queue::sync called recursively on the same serial queue!");
        }

        {
            std::unique_lock<std::mutex> lock(mutex_);
            tasks_.push(wrapper);

            if (!isRunning_) {
                isRunning_ = true;
                submit_next();
            }
        }
    }

    std::unique_lock<std::mutex> lock(m);
    cv.wait(lock, [&] { return done; });
}
 */

void Queue::sync(Task task) {
    if (type_ == Type::Concurrent) {
        std::mutex m;
        std::condition_variable cv;
        bool done = false;

        ThreadPool::instance().submit([&]{
            task();
            {
                std::lock_guard<std::mutex> lock(m);
                done = true;
            }
            cv.notify_one();
        }, qos_);

        std::unique_lock<std::mutex> lock(m);
        cv.wait(lock, [&]{ return done; });
    } else {
        if (std::this_thread::get_id() == running_thread_id_) {
            assert(false && "Queue::sync called recursively on the same serial queue!");
        }

        running_thread_id_ = std::this_thread::get_id();
        try {
            task();
        } catch (...) {
            std::cerr << "Queue[" << name_ << "] exception in sync\n";
        }
        running_thread_id_ = std::thread::id{};
    }
}

void Queue::submit_next() {
    if (tasks_.empty()) {
        isRunning_ = false;
        return;
    }

    auto task = std::move(tasks_.front());
    tasks_.pop();

    ThreadPool::instance().submit([this, task = std::move(task)]() mutable {
        running_thread_id_ = std::this_thread::get_id();
        try {
            task();
        } catch (...) {
            std::cerr << "Queue[" << name_ << "] exception\n";
        }
        running_thread_id_ = std::thread::id{};
        submit_next();
    }, qos_);
}

}

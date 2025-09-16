#include "thread_pool.hpp"

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

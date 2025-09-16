#include <TurboQ/timer.hpp>
#include <TurboQ/queue.hpp>

namespace turboq {

Timer& Timer::instance() {
    static Timer tq;
    return tq;
}

void Timer::schedule(Task task,
                          std::chrono::steady_clock::time_point when,
                          turboq::Queue& queue) {
    {
        std::unique_lock<std::mutex> lock(mutex_);
        tasks_.emplace(ScheduledTask{when, std::move(task), &queue});
    }
    cv_.notify_one();
}

Timer::Timer() {
    worker_ = std::thread([this] { run(); });
}

Timer::~Timer() {
    {
        std::unique_lock<std::mutex> lock(mutex_);
        stop_ = true;
    }
    cv_.notify_one();
    if (worker_.joinable()) worker_.join();
}

void Timer::run() {
    std::unique_lock<std::mutex> lock(mutex_);
    while (!stop_) {
        if (tasks_.empty()) {
            cv_.wait(lock, [this] { return stop_ || !tasks_.empty(); });
        } else {
            auto next = tasks_.top();
            if (cv_.wait_until(lock, next.when, [this, &next] {
                    return stop_ || tasks_.top().when < next.when;
                })) {
                continue;
            }

            tasks_.pop();
            lock.unlock();
            next.queue->async(std::move(next.task));
            lock.lock();
        }
    }
}

} 

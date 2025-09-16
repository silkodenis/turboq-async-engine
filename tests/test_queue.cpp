#include <catch2/catch_test_macros.hpp>
#include "turboq.hpp"

#include <atomic>
#include <vector>
#include <chrono>
#include <thread>

using namespace turboq;
using namespace std::chrono_literals;

TEST_CASE("Queue async executes tasks", "[Queue]") {
    Queue sut("test_concurrent", Queue::Type::Concurrent, ThreadPool::QoS::Utility);
    std::atomic<int> counter{0};

    sut.async([&] { counter++; });
    sut.async([&] { counter++; });

    std::this_thread::sleep_for(100ms);
    REQUIRE(counter == 2);
}

TEST_CASE("Queue sync executes task immediately", "[Queue]") {
    Queue sut("test_serial", Queue::Type::Serial, ThreadPool::QoS::Utility);
    std::atomic<int> counter{0};

    sut.sync([&] { counter++; });
    sut.sync([&] { counter++; });

    REQUIRE(counter == 2);
}

TEST_CASE("Queue Serial executes tasks in order", "[Queue]") {
    Queue sut("serial_test", Queue::Type::Serial, ThreadPool::QoS::Utility);
    std::vector<int> order;
    std::mutex m;

    for (int i = 0; i < 5; i++) {
        sut.async([&, i] {
            std::lock_guard<std::mutex> lock(m);
            order.push_back(i);
        });
    }

    std::this_thread::sleep_for(200ms);
    REQUIRE(order.size() == 5);
    for (int i = 0; i < 5; i++) {
        REQUIRE(order[i] == i);
    }
}

TEST_CASE("Queue async_after executes after delay", "[Queue]") {
    Queue sut("timer_test", Queue::Type::Concurrent, ThreadPool::QoS::Utility);
    std::atomic<bool> executed{false};

    auto start = std::chrono::steady_clock::now();
    sut.async_after(100ms, [&] {
        executed = true;
    });

    while (!executed) {
        std::this_thread::sleep_for(10ms);
    }

    auto duration = std::chrono::steady_clock::now() - start;
    REQUIRE(duration >= 90ms);
}

TEST_CASE("Queue async_at executes at specific time", "[Queue]") {
    Queue sut("timer_at_test", Queue::Type::Concurrent, ThreadPool::QoS::Utility);
    std::atomic<bool> executed{false};

    auto when = std::chrono::steady_clock::now() + 100ms;
    sut.async_at(when, [&] { executed = true; });

    while (!executed) {
        std::this_thread::sleep_for(10ms);
    }

    auto now = std::chrono::steady_clock::now();
    REQUIRE(now >= when);
}

TEST_CASE("Queue global concurrent queues work", "[Queue]") {
    auto& sut = Queue::global(ThreadPool::QoS::UserInteractive);
    std::atomic<int> counter{0};

    sut.async([&] { counter++; });
    sut.async([&] { counter++; });

    std::this_thread::sleep_for(100ms);
    REQUIRE(counter == 2);
}

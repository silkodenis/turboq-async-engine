#include <catch2/catch_test_macros.hpp>
#include <TurboQ/thread_pool.hpp>
#include "test_helpers.hpp"

#include <atomic>
#include <chrono>
#include <thread>

using namespace turboq;
using namespace std::chrono_literals;

TEST_CASE("ThreadPool executes tasks", "[ThreadPool]") {
    std::atomic<int> counter{0};
    ThreadPool sut(2);

    sut.submit([&]{ counter++; }, ThreadPool::QoS::Utility);
    sut.submit([&]{ counter++; }, ThreadPool::QoS::Utility);

    REQUIRE(test_helpers::wait_until([&]{ return counter == 2; }));
}

TEST_CASE("ThreadPool stops cleanly", "[ThreadPool]") {
    ThreadPool sut(2);
    std::atomic<bool> done{false};

    sut.submit([&]{ done = true; });

    REQUIRE(test_helpers::wait_until([&]{ return done.load(); }));
}

TEST_CASE("ThreadPool prioritizes higher QoS over lower QoS in long run", "[ThreadPool]") {
    ThreadPool sut(10);

    struct Record { ThreadPool::QoS qos; int index; };
    std::vector<Record> executed;
    std::mutex m;

    constexpr int iterations = 30;

    for (int i = 0; i < iterations; i++) {
        sut.submit([&, i]{ std::lock_guard<std::mutex> lock(m); executed.push_back({ThreadPool::QoS::UserInteractive, i}); }, ThreadPool::QoS::UserInteractive);
        sut.submit([&, i]{ std::lock_guard<std::mutex> lock(m); executed.push_back({ThreadPool::QoS::UserInitiated, i}); }, ThreadPool::QoS::UserInitiated);
        sut.submit([&, i]{ std::lock_guard<std::mutex> lock(m); executed.push_back({ThreadPool::QoS::Utility, i}); }, ThreadPool::QoS::Utility);
        sut.submit([&, i]{ std::lock_guard<std::mutex> lock(m); executed.push_back({ThreadPool::QoS::Background, i}); }, ThreadPool::QoS::Background);
    }

    REQUIRE(test_helpers::wait_until([&]{
        std::lock_guard<std::mutex> lock(m);
        return executed.size() == iterations * 4;
    }, 5000ms));

    auto qos_value = [](ThreadPool::QoS q){ return static_cast<int>(q); };

    constexpr int tail_size = 20;
    int violations = 0;

    {
        std::lock_guard<std::mutex> lock(m);
        for (size_t i = executed.size() - tail_size; i + 1 < executed.size(); i++) {
            if (qos_value(executed[i].qos) < qos_value(executed[i+1].qos)) {
                violations++;
            }
        }
    }

    REQUIRE(violations <= 10);
}

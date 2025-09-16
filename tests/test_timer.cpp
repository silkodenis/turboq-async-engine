#include <catch2/catch_test_macros.hpp>
#include <TurboQ/timer.hpp>
#include <TurboQ/queue.hpp>

#include <atomic>
#include <chrono>
#include <thread>

using namespace std::chrono_literals;

TEST_CASE("Timer executes task after delay", "[Timer]") {
    auto& sut = turboq::Timer::instance();
    turboq::Queue queue("test", turboq::Queue::Type::Serial);

    std::atomic<bool> executed{false};

    auto when = std::chrono::steady_clock::now() + 50ms;

    sut.schedule([&] {
        executed = true;
    }, when, queue);

    std::this_thread::sleep_for(100ms);

    REQUIRE(executed.load());
}

TEST_CASE("Timer executes multiple tasks in correct order", "[Timer]") {
    auto& sut = turboq::Timer::instance();
    turboq::Queue queue("test", turboq::Queue::Type::Serial);

    std::vector<int> order;
    std::mutex m;

    auto now = std::chrono::steady_clock::now();
    sut.schedule([&] { std::lock_guard<std::mutex> l(m); order.push_back(1); }, now + 30ms, queue);
    sut.schedule([&] { std::lock_guard<std::mutex> l(m); order.push_back(2); }, now + 10ms, queue);
    sut.schedule([&] { std::lock_guard<std::mutex> l(m); order.push_back(3); }, now + 20ms, queue);

    std::this_thread::sleep_for(100ms);

    REQUIRE(order.size() == 3);
    REQUIRE(order[0] == 2);
    REQUIRE(order[1] == 3);
    REQUIRE(order[2] == 1);
}

TEST_CASE("Timer does not execute before scheduled time", "[Timer]") {
    auto& sut = turboq::Timer::instance();
    turboq::Queue queue("test", turboq::Queue::Type::Serial);

    std::atomic<bool> executed{false};

    auto when = std::chrono::steady_clock::now() + 200ms;

    sut.schedule([&] {
        executed = true;
    }, when, queue);

    std::this_thread::sleep_for(50ms);
    REQUIRE_FALSE(executed.load());

    std::this_thread::sleep_for(200ms);
    REQUIRE(executed.load());
}

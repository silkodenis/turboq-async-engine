#include <catch2/catch_test_macros.hpp>
#include <TurboQ/thread_pool.hpp>

#include <atomic>
#include <chrono>
#include <thread>

using namespace std::chrono_literals;

TEST_CASE("ThreadPool executes tasks", "[ThreadPool]") {
    std::atomic<int> counter{0};

    auto sut = turboq::ThreadPool(2);

    sut.submit([&] { counter++; }, turboq::ThreadPool::QoS::Utility);
    sut.submit([&] { counter++; }, turboq::ThreadPool::QoS::Utility);

    std::this_thread::sleep_for(100ms);

    REQUIRE(counter == 2);
}

TEST_CASE("ThreadPool stops cleanly", "[ThreadPool]") {
    auto sut = turboq::ThreadPool(2);
    
    std::atomic<bool> done{false};
    sut.submit([&] {
        std::this_thread::sleep_for(50ms);
        done = true;
    });

    std::this_thread::sleep_for(100ms);
    REQUIRE(done);
}

TEST_CASE("ThreadPool prioritizes higher QoS over lower QoS in long run", "[ThreadPool]") {
    using namespace turboq;
    
    auto sut = turboq::ThreadPool(10);

    struct Record {
        ThreadPool::QoS qos;
        int index;
    };

    std::vector<Record> executed;
    std::mutex m;

    constexpr int iterations = 30;

    for (int i = 0; i < iterations; i++) {
        sut.submit([&, i] {
            std::lock_guard<std::mutex> lock(m);
            executed.push_back({ThreadPool::QoS::UserInteractive, i});
        }, ThreadPool::QoS::UserInteractive);

        sut.submit([&, i] {
            std::lock_guard<std::mutex> lock(m);
            executed.push_back({ThreadPool::QoS::UserInitiated, i});
        }, ThreadPool::QoS::UserInitiated);

        sut.submit([&, i] {
            std::lock_guard<std::mutex> lock(m);
            executed.push_back({ThreadPool::QoS::Utility, i});
        }, ThreadPool::QoS::Utility);

        sut.submit([&, i] {
            std::lock_guard<std::mutex> lock(m);
            executed.push_back({ThreadPool::QoS::Background, i});
        }, ThreadPool::QoS::Background);
    }

    std::this_thread::sleep_for(3s);

    REQUIRE(executed.size() == iterations * 4);

    auto qos_value = [](ThreadPool::QoS q) {
        return static_cast<int>(q);
    };

    constexpr int tail_size = 20;
    REQUIRE(executed.size() > tail_size);

    int violations = 0;
    for (size_t i = executed.size() - tail_size; i + 1 < executed.size(); i++) {
        if (qos_value(executed[i].qos) < qos_value(executed[i+1].qos)) {
            violations++;
        }
    }

    REQUIRE(violations <= 10);
}

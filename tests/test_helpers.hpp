#pragma once
#include <chrono>
#include <thread>
#include <functional>

namespace test_helpers {

inline bool wait_until(std::function<bool()> condition,
                       std::chrono::milliseconds timeout = std::chrono::milliseconds(1000),
                       std::chrono::milliseconds interval = std::chrono::milliseconds(1))
{
    auto start = std::chrono::steady_clock::now();
    while (!condition()) {
        if (std::chrono::steady_clock::now() - start > timeout)
            return false;
        std::this_thread::sleep_for(interval);
    }
    return true;
}

}

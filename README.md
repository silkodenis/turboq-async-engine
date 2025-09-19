[![License](https://img.shields.io/github/license/silkodenis/turboq-cpp-async-engine.svg)](https://github.com/silkodenis/turboq-cpp-async-engine/blob/main/LICENSE)
![Linux](https://github.com/silkodenis/turboq-cpp-async-engine/actions/workflows/ci-linux.yml/badge.svg?branch=main)
![macOS](https://github.com/silkodenis/turboq-cpp-async-engine/actions/workflows/ci-macos.yml/badge.svg?branch=main)
![Windows](https://github.com/silkodenis/turboq-cpp-async-engine/actions/workflows/ci-windows.yml/badge.svg?branch=main)

# TurboQ

C++ library providing asynchronous task and timer queues. It relies on a fixed-size thread pool, minimizing overhead from thread creation and context switching. This architecture enables efficient scheduling and execution of concurrent and serial workloads. The design is inspired by Apple’s Grand Central Dispatch (GCD) and implemented solely with standard C++ primitives: `std::thread`, `std::mutex`, and `std::condition_variable`.

The library provides:
- **ThreadPool** with task priority (QoS) support for parallel execution.
- **Serial and Concurrent queues** for ordered or asynchronous task execution.
- **Timers** for delayed or scheduled task execution.
- Easy integration via **CMake** and optional Git submodule.

## Requirements
- **Git** 
- **CMake** ≥ 3.20
- **C++17** compatible compiler (GCC, Clang, MSVC)  

## Using with CMake FetchContent

You can fetch the library directly in your CMakeLists.txt:

```cmake
include(FetchContent)

FetchContent_Declare(
  TurboQ
  GIT_REPOSITORY https://github.com/silkodenis/turboq-cpp-async-engine.git
  GIT_TAG        main
)

FetchContent_MakeAvailable(TurboQ)

target_link_libraries(MyApp PRIVATE turboq)
```

## Using as a Git submodule

To include **TurboQ** in your project as a Git submodule:

```bash
git submodule add https://github.com/silkodenis/turboq-cpp-async-engine.git external/TurboQ
git submodule update --init --recursive
```

Then, in your CMakeLists.txt:

```cmake
# Add TurboQ submodule
add_subdirectory(external/TurboQ)

# Link with your target
target_link_libraries(MyApp PRIVATE turboq)
```

## Manual build

#### 1. Clone repository and initialize submodules

```bash
git clone https://github.com/silkodenis/turboq-cpp-async-engine.git
cd turboq-cpp-async-engine

#  If you plan to build and run tests (-DBUILD_TESTS=ON), initialize submodules as well:
git submodule update --init --recursive
```

#### 2. Configure, build library, and run tests

```bash
mkdir build && cd build
cmake .. -DBUILD_TESTS=ON -DBUILD_SHARED=OFF  
cmake --build . --parallel

# optional, only if BUILD_TESTS=ON
ctest -V   
```

#### Configure options:

The following options can be set when configuring the project:

- `BUILD_TESTS` (default: `ON`) - enables building and running tests (requires Catch2 submodule)
- `BUILD_SHARED` (default: `OFF`) - build library as shared (ON) or static (OFF)

## Example

```cpp
#include <TurboQ/turboq.hpp>
#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>

using namespace std::chrono_literals;
using namespace turboq;

int main() {
    std::mutex cout_mutex;

    // Concurrent queue (tasks may run in parallel)
    Queue concurrent("concurrent", Queue::Type::Concurrent, ThreadPool::QoS::Utility);

    for (int i = 1; i <= 5; i++) {
        concurrent.async([i, &cout_mutex] {
            std::lock_guard<std::mutex> lock(cout_mutex);
            std::cout << "Concurrent task " << i
                      << " running on thread "
                      << std::this_thread::get_id() << std::endl;
            std::this_thread::sleep_for(200ms);
        });
    }

    // Serial queue (tasks run one after another)
    Queue serial("serial", Queue::Type::Serial);

    serial.async([&cout_mutex] {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << "Serial task 1" << std::endl;
    });

    serial.async([&cout_mutex] {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << "Serial task 2" << std::endl;
    });

    // Delayed task (executes after 1 second)
    concurrent.async_after(1s, [&cout_mutex] {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << "Delayed task executed after 1s" << std::endl;
    });

    // Synchronous task (blocks the current thread until finished)
    serial.sync([&cout_mutex] {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << "Synchronous task finished" << std::endl;
    });

    // Wait a bit to let all tasks complete
    std::this_thread::sleep_for(3s);
}
```

## License

This project is licensed under the [Apache License 2.0](https://github.com/silkodenis/turboq-cpp-async-engine/blob/main/LICENSE).

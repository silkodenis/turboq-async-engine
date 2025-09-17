[![License](https://img.shields.io/github/license/silkodenis/turboq-async-engine.svg)](https://github.com/silkodenis/turboq-async-engine/blob/main/LICENSE)

| Platform | Build & Tests |
|----------|--------|
| Linux    | ![Linux](https://github.com/silkodenis/turboq-async-engine/actions/workflows/ci.yml/badge.svg?branch=main&job=linux) |
| macOS    | ![macOS](https://github.com/silkodenis/turboq-async-engine/actions/workflows/ci.yml/badge.svg?branch=main&job=macos) |
| Windows  | ![Windows](https://github.com/silkodenis/turboq-async-engine/actions/workflows/ci.yml/badge.svg?branch=main&job=windows) |

# TurboQ

Asynchronous task and timer queue library for C++.

## Requirements
- **Git** (for submodules)
- **CMake** ≥ 3.20
- **C++17** compatible compiler (GCC, Clang, MSVC)  


## Using with CMake FetchContent

You can fetch the library directly in your CMakeLists.txt:

```cmake
include(FetchContent)

FetchContent_Declare(
  TurboQ
  GIT_REPOSITORY https://github.com/silkodenis/turboq-async-engine.git
  GIT_TAG        main
)

FetchContent_MakeAvailable(TurboQ)

target_link_libraries(MyApp PRIVATE turboq)
```

## Using as a Git submodule

To include **TurboQ** in your project as a Git submodule:

```bash
git submodule add https://github.com/silkodenis/turboq-async-engine.git external/TurboQ
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
git clone https://github.com/silkodenis/turboq-async-engine.git
cd turboq-async-engine
git submodule update --init --recursive
```

#### 2. Configure, build library, and run tests

```bash
mkdir build && cd build
cmake .. -DBUILD_TESTS=ON -DBUILD_SHARED=OFF 
cmake --build . --parallel
ctest -V
```

#### Configure options:

The following options can be set when configuring the project:

- `BUILD_TESTS` (default: `ON`)
- `BUILD_SHARED` (default: `OFF`)

## License

This project is licensed under the [Apache License 2.0](https://github.com/silkodenis/turboq-async-engine/blob/main/LICENSE).

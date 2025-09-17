# TurboQ

Asynchronous task and timer queue library for C++.

## Requirements
- **Git** (for submodules)
- **CMake** ≥ 3.20
- **C++17** compatible compiler (GCC, Clang, MSVC)  

## Build instructions

#### 1. Clone repository and initialize submodules

```bash
git clone https://github.com/silkodenis/turboq-async-engine.git
cd cpp-turboq-async-engine
git submodule update --init --recursive
```

#### 2. Configure, build library, and run tests

```bash
mkdir build && cd build
cmake .. -DBUILD_TESTS=ON -DBUILD_SHARED=ON
cmake --build . --parallel
ctest -V
```

#### Configure options:

The following options can be set when configuring the project:

- `BUILD_TESTS` (default: `ON`)
- `BUILD_SHARED` (default: `OFF`)

## Using TurboQ as a Git submodule

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

## Using TurboQ with CMake FetchContent

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

## License

This project is licensed under the [Apache License 2.0](https://github.com/silkodenis/turboq-async-engine/blob/main/LICENSE).


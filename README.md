# TurboQ

Asynchronous task and timer queue library for C++.

## Requirements
- **Git** (for submodules)
- **CMake** ≥ 3.20
- **C++17** compatible compiler (GCC, Clang, MSVC)  
- (Optional) **Ninja** build system for faster builds  

## Build instructions

#### 1. Clone repository and initialize submodules

```bash
git clone https://github.com/silkodenis/cpp-turboq-async-engine.git
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

## License

This project is licensed under the [Apache License 2.0](https://github.com/silkodenis/cpp-turboq-async-engine/blob/main/LICENSE).


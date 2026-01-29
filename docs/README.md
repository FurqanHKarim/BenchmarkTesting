# Container Benchmarks

This repository is designed to benchmark and compare various C++ containers and features, focusing on performance and cache efficiency.

## Goals
- Compare `std` containers with custom or alternative implementations.
- Analyze cache hit/miss impact on performance.
- Document findings on what works fast vs. slow.

## Building and Running

### Prerequisites
- C++ Compiler (GCC, Clang, or MSVC)
- CMake (3.14+)
- Ninja (optional but recommended for faster builds)

### Build Steps

1. **Configure**:
   ```bash
   cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
   ```
   *Note: If you don't have Ninja, you can omit `-G Ninja` and use the default generator (e.g., MinGW Makefiles or Visual Studio).*

2. **Build**:
   ```bash
   cmake --build build
   ```

3. **Run**:
   ```bash
   .\build\container_benchmarks.exe
   ```

## Adding Benchmarks
Add new benchmark functions in `src/main.cpp` (or split into multiple files) and use the `BENCHMARK` macro to register them.

cmake_minimum_required(VERSION 3.16)

project(ContainerBenchmarks LANGUAGES CXX)

set(CMAKE_GENERATOR "Ninja" CACHE STRING "" FORCE)

set(CMAKE_POLICY_DEFAULT_CMP0048 NEW) # For older project versions in FetchContent

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Enable testing of the benchmark library." FORCE)
set(BENCHMARK_ENABLE_WERROR OFF CACHE BOOL "Enable -Werror." FORCE)

include(FetchContent)

# Google Benchmark
FetchContent_Declare(
  googlebenchmark
  GIT_REPOSITORY https://github.com/google/benchmark.git
  GIT_TAG v1.8.3
)
FetchContent_MakeAvailable(googlebenchmark)

# Abseil
FetchContent_Declare(
  abseil-cpp
  GIT_REPOSITORY https://github.com/abseil/abseil-cpp.git
  GIT_TAG 20240116.1
)
# Suppress Abseil install rules to avoid conflicts
set(ABSL_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
set(ABSL_PROPAGATE_CXX_STD ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(abseil-cpp)

# TSL Robin Map
# TSL Robin Map - REMOVED due to CMake compatibility issues
# FetchContent_Declare(
#   tsl-robin-map
#   GIT_REPOSITORY https://github.com/Tessil/robin-map.git
#   GIT_TAG v1.3.0
# )
# FetchContent_MakeAvailable(tsl-robin-map)

# TSL Hopscotch Map - REMOVED
# FetchContent_Declare(
#   tsl-hopscotch-map
#   GIT_REPOSITORY https://github.com/Tessil/hopscotch-map.git
#   GIT_TAG v2.3.1
# )
# FetchContent_MakeAvailable(tsl-hopscotch-map)

# TSL Ordered Map - REMOVED
# FetchContent_Declare(
#   tsl-ordered-map
#   GIT_REPOSITORY https://github.com/Tessil/ordered-map.git
#   GIT_TAG v1.1.0
# )
# FetchContent_MakeAvailable(tsl-ordered-map)

# Martinus Robin Hood
FetchContent_Declare(
  robin-hood-hashing
  GIT_REPOSITORY https://github.com/martinus/robin-hood-hashing.git
  GIT_TAG 3.11.5
)
FetchContent_MakeAvailable(robin-hood-hashing)

# Parallel Hashmap
FetchContent_Declare(
  parallel-hashmap
  GIT_REPOSITORY https://github.com/greg7mdp/parallel-hashmap.git
  GIT_TAG 1.3.9
)
FetchContent_MakeAvailable(parallel-hashmap)

add_executable(container_benchmarks src/hashmap_benchmarks.cpp)

target_link_libraries(container_benchmarks PRIVATE 
    benchmark::benchmark 
    benchmark::benchmark_main
    absl::flat_hash_map
    # tsl::robin_map
    # tsl::hopscotch_map
    # tsl::ordered_map
    phmap
)

# Robin Hood is header-only without a target in older versions or specific setups, 
# so we include the directory manually if the target doesn't exist. 
# But usually FetchContent allows accessing the source dir.
target_include_directories(container_benchmarks PRIVATE 
    ${robin-hood-hashing_SOURCE_DIR}/src/include
)

# Random Access Benchmarks
add_executable(random_access_benchmarks src/hashmap_random_access.cpp)

target_link_libraries(random_access_benchmarks PRIVATE 
    benchmark::benchmark 
    benchmark::benchmark_main
    absl::flat_hash_map
    phmap
)

target_include_directories(random_access_benchmarks PRIVATE 
    ${robin-hood-hashing_SOURCE_DIR}/src/include
)

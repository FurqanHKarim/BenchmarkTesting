/**
 * @file hashmap_benchmarks.cpp
 * @brief Benchmark comparisons between various C++ hashmap implementations.
 * 
 * This file contains benchmarks for:
 * - std::unordered_map (Standard Library)
 * - absl::flat_hash_map (Google Abseil)
 * - robin_hood::unordered_map (Martinus Robin Hood)
 * - phmap::flat_hash_map (Parallel Hashmap)
 * 
 * Benchmarks cover:
 * - Histogram Sort: Measuring insertion performance and frequency counting.
 */

#include <benchmark/benchmark.h>
#include <vector>
#include <numeric>
#include <algorithm>
#include <random>
#include <unordered_map>
#include "absl/container/flat_hash_map.h"
#include "robin_hood.h"
#include "parallel_hashmap/phmap.h"

/**
 * @brief Generates a vector of integers in ascending order.
 * @param size Number of elements to generate.
 * @return std::vector<int> Vector containing [0, 1, ..., size-1].
 */
std::vector<int> GenerateAscendingData(size_t size) {
    std::vector<int> data(size);
    std::iota(data.begin(), data.end(), 0);
    return data;
}

/**
 * @brief Generates a vector of integers in descending order.
 * @param size Number of elements to generate.
 * @return std::vector<int> Vector containing [size-1, size-2, ..., 0].
 */
std::vector<int> GenerateDescendingData(size_t size) {
    std::vector<int> data(size);
    std::iota(data.rbegin(), data.rend(),0);
    return data;
}

/**
 * @brief Generates a vector of random integers.
 * Uses a fixed seed (42) for reproducible benchmark results.
 * @param size Number of elements to generate.
 * @return std::vector<int> Vector containing random integers between 0 and size.
 */
std::vector<int> GenerateRandomData(size_t size) {
    std::vector<int> data(size);
    std::mt19937 gen(42); // Fixed seed for reproducibility
    std::uniform_int_distribution<> dis(0, static_cast<int>(size));
    for (auto& val : data) {
        val = dis(gen);
    }
    return data;
}

/**
 * @brief Performs a histogram sort using the specified Hashmap type.
 * 
 * Counts the frequency of each number in the input data and then
 * reconstructs the data vector based on the sorted order of keys
 * (implicitly sorted if using an ordered map, but here we iterate 
 * through the hashmap which is unordered, so the output order 
 * depends on the hashmap's iteration order).
 * 
 * @tparam Hashmap The hashmap implementation to use (e.g., std::unordered_map).
 * @param data Input vector of integers to sort/count.
 * @return std::vector<int> The reconstructed vector (modified in place).
 */
template<typename Hashmap>
std::vector<int> histogramSort(std::vector<int>& data){
    Hashmap sorted;
    for(int val : data){
        sorted[val]++;
    }

    int index = 0;
    for (auto i = sorted.begin(); i != sorted.end(); i++)
    {
        for (int j = 0; j < i->second; ++j) {
            data[index++] = i->first;
        }
    }
    
    return data;
}

/**
 * @brief Benchmark function for Histogram Sort.
 * 
 * Measures the time taken to perform the histogram sort operation
 * on a copy of the random data.
 * 
 * @tparam Hashmap The hashmap implementation to benchmark.
 * @param state Google Benchmark state object.
 */
template<typename Hashmap>
static void BM_HistogramSort(benchmark::State& state){
    auto data = GenerateRandomData(state.range(0));
    for (auto _ : state) {
        state.PauseTiming();
        std::vector<int> copy = data;
        state.ResumeTiming();
        histogramSort<Hashmap>(copy);
    }
    state.SetComplexityN(state.range(0));
}

// Register benchmarks
BENCHMARK_TEMPLATE(BM_HistogramSort, std::unordered_map<int, int>)->Range(256, 1<<16)->Complexity();
BENCHMARK_TEMPLATE(BM_HistogramSort, absl::flat_hash_map<int, int>)->Range(256, 1<<16)->Complexity();
BENCHMARK_TEMPLATE(BM_HistogramSort, robin_hood::unordered_map<int, int>)->Range(256, 1<<16)->Complexity();
BENCHMARK_TEMPLATE(BM_HistogramSort, phmap::flat_hash_map<int, int>)->Range(256, 1<<16)->Complexity();

BENCHMARK_MAIN();



/*
| Library                | Type                  | Notes                             |
| ---------------------- | --------------------- | --------------------------------- |
| `std::unordered_map`   | Standard              | Baseline                          |
| `absl::flat_hash_map`  | High-perf swiss table | Common baseline faster than std   |
| `tsl::robin_map`       | Robin Hood            | Header-only, power/primes         |
| `tsl::hopscotch_map`   | Hopscotch             | Cache-friendly                    |
| `tsl::ordered_map`     | Ordered               | Maintains insertion order         |
| `martinus::robin_hood` | Robin Hood            | Fast, minimal                     |
| `parallel-hashmap`     | High-perf             | Memory friendly, concurrent focus |
| `folly::F14FastMap`    | Facebook              | Prehashing, SIMD                  |
*/
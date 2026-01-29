#include <benchmark/benchmark.h>
#include <vector>
#include <numeric>
#include <algorithm>
#include <random>
#include <unordered_map>
#include "absl/container/flat_hash_map.h"
#include "robin_hood.h"
#include "parallel_hashmap/phmap.h"

// Helper function to generate Ascending Data
std::vector<int> GenerateAscendingData(size_t size) {
    std::vector<int> data(size);
    std::iota(data.begin(), data.end(), 0);
    return data;
}

// Helper function to generate Descending Data
std::vector<int> GenerateDescendingData(size_t size) {
    std::vector<int> data(size);
    std::iota(data.rbegin(), data.rend(),0);
    return data;
}

// Helper function to generate Random Data
std::vector<int> GenerateRandomData(size_t size) {
    std::vector<int> data(size);
    std::mt19937 gen(42); // Fixed seed for reproducibility
    std::uniform_int_distribution<> dis(0, static_cast<int>(size));
    for (auto& val : data) {
        val = dis(gen);
    }
    return data;
}

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
#include <benchmark/benchmark.h>
#include <vector>
#include <numeric>
#include <algorithm>
#include <random>
#include <unordered_map>
#include "absl/container/flat_hash_map.h"
#include "robin_hood.h"
#include "parallel_hashmap/phmap.h"

// Helper function to generate Random Data
// We generate 2*size range to ensure some spread, but we return 'size' elements.
std::vector<int> GenerateRandomData(size_t size) {
    std::vector<int> data(size);
    std::mt19937 gen(42); // Fixed seed for reproducibility
    std::uniform_int_distribution<> dis(0, static_cast<int>(size) * 2); 
    for (auto& val : data) {
        val = dis(gen);
    }
    return data;
}

template<typename Hashmap>
static void BM_RandomAccess(benchmark::State& state) {
    const size_t size = state.range(0);
    // Generate data
    auto data = GenerateRandomData(size);
    
    // Setup map (not timed)
    Hashmap map;
    map.reserve(size); // Reserve to avoid rehash during insertion if possible
    for (int val : data) {
        map[val] = val; 
    }
    
    // Prepare lookup keys: we use the inserted data but shuffled
    // to simulate random access patterns to existing keys.
    std::vector<int> lookups = data;
    std::mt19937 gen(123);
    std::shuffle(lookups.begin(), lookups.end(), gen);
    
    size_t lookup_idx = 0;
    const size_t lookup_mask = lookups.size() - 1; // Used if size is power of 2, else use modulo
    
    for (auto _ : state) {
        // Get next key to lookup
        int key = lookups[lookup_idx];
        
        // Perform lookup
        benchmark::DoNotOptimize(map.find(key));
        
        // Advance index
        lookup_idx++;
        if (lookup_idx >= lookups.size()) {
            lookup_idx = 0;
        }
    }
    
    state.SetComplexityN(state.range(0));
    state.SetItemsProcessed(state.iterations());
}

BENCHMARK_TEMPLATE(BM_RandomAccess, std::unordered_map<int, int>)->Range(256, 1<<20)->Complexity();
BENCHMARK_TEMPLATE(BM_RandomAccess, absl::flat_hash_map<int, int>)->Range(256, 1<<20)->Complexity();
BENCHMARK_TEMPLATE(BM_RandomAccess, robin_hood::unordered_map<int, int>)->Range(256, 1<<20)->Complexity();
BENCHMARK_TEMPLATE(BM_RandomAccess, phmap::flat_hash_map<int, int>)->Range(256, 1<<20)->Complexity();

BENCHMARK_MAIN();

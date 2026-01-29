# Benchmark Results: Hashmap Performance in Histogram Sort

**Date:** 2026-01-29  
**Build Type:** Release (Ninja Generator)  
**CPU:** 2611 MHz (12 Cores)

## 1. Executive Summary
The benchmark compared the performance of four C++ hashmap implementations in a frequency counting task ("Histogram Sort"). 

**Key Finding:** Modern "flat" hashmaps (`phmap` and `absl`) significantly outperformed the standard library, running **~3.5x faster** than `std::unordered_map` for large datasets (N=65,536).

## 2. Methodology
*   **Algorithm**: `HistogramSort` - iterates through a vector of integers, counts frequencies using a hashmap, and reconstructs the sorted vector.
*   **Input Data**: Random integers.
*   **Input Sizes**: 256 to 65,536 elements.
*   **Metric**: CPU Time (nanoseconds).

## 3. Results (at N = 65,536)

| Rank | Library | Type | CPU Time (ns) | Relative Speed |
| :--- | :--- | :--- | :--- | :--- |
| **1** | **`phmap::flat_hash_map`** | Flat / Open Addressing | **~475,324** | **1.0x (Baseline)** |
| **2** | `absl::flat_hash_map` | Flat / Open Addressing | ~515,625 | 1.08x slower |
| **3** | `robin_hood::unordered_map` | Open Addressing | ~1,360,212 | 2.86x slower |
| **4** | `std::unordered_map` | Node-based / Chaining | ~1,650,799 | 3.47x slower |

## 4. Detailed Analysis

### Why Flat Maps Won
Both **Parallel Hashmap (`phmap`)** and **Abseil (`absl`)** use "open addressing" with a flat memory layout.
*   **Cache Locality**: They store data directly in a contiguous array. When the CPU fetches one bucket, it pre-fetches neighbors, making subsequent lookups for collisions extremely fast.
*   **No Allocations**: Unlike `std::unordered_map`, they don't allocate a separate heap node for every single entry. This reduces memory fragmentation and allocator overhead.

### The Standard Library (`std::unordered_map`)
*   **Performance**: It was the slowest performer.
*   **Reason**: The C++ standard requires pointer stability for elements, forcing implementations to use a "node-based" approach (linked lists for buckets). This destroys cache locality, as every access is a "pointer chase" to a random heap location.

### Robin Hood Hashing
*   **Performance**: While generally a high-performance map, `robin_hood` lagged behind the Google-derived maps (Abseil/Phmap) in this specific histogram workload.
*   **Observation**: It was still ~20% faster than the standard library but didn't match the optimized SIMD/probing strategies of `absl` and `phmap` for this specific data distribution.

## 5. Build Configuration Note
These results were obtained in **Release** mode.
*   In **Debug** mode, `absl` and `phmap` were significantly slower (up to 40ms) due to the lack of inlining for their complex template machinery.
*   **Conclusion**: Always benchmark heavy template libraries in Release mode.

## 6. Random Access Benchmark Results

**Methodology**:
*   **Test**: Random Access (Lookup) Speed.
*   **Procedure**: Insert N random integers into the map. Then, perform M lookups using a shuffled version of the inserted keys (ensuring 100% hit rate, simulating random access).
*   **Metric**: CPU Time per lookup (nanoseconds).

**Results (at N = 1,048,576 / 1 Million items):**

| Rank | Library | CPU Time (ns) | Relative Speed |
| :--- | :--- | :--- | :--- |
| **1** | **`phmap::flat_hash_map`** | **~5.31 ns** | **1.0x (Baseline)** |
| **2** | `absl::flat_hash_map` | ~6.08 ns | 1.14x slower |
| **3** | `robin_hood::unordered_map` | ~7.11 ns | 1.33x slower |
| **4** | `std::unordered_map` | ~16.60 ns | 3.12x slower |

**Analysis**:
*   **Consistent Dominance**: Similar to the histogram test, `phmap` and `absl` dominate due to superior cache locality (Swiss Table design).
*   **Cache Miss Cost**: `std::unordered_map` suffers heavily at large sizes (1M items) because its node-based structure causes a cache miss on almost every access.
*   **Robin Hood**: Performs very well, sitting between the Swiss Tables and the Standard Library.

# Benchmark Results: Hashmap Performance in Histogram Sort

**Report Generated:** 2026-02-04
**Build Type:** Release (Ninja Generator)
**CPU:** 2611 MHz (12 Cores)

## 1. Executive Summary
The benchmark compared the performance of four C++ hashmap implementations in a frequency counting task ("Histogram Sort") and a Random Access lookup task.

**Key Finding:** Modern "flat" hashmaps (`absl` and `phmap`) significantly outperformed the standard library.
- **Histogram Sort**: `absl::flat_hash_map` was the fastest, running **~2.68x faster** than `std::unordered_map`.
- **Random Access**: `phmap::flat_hash_map` was the fastest, running **~2.51x faster** than `std::unordered_map`.

## 2. Methodology
*   **Algorithm**: `HistogramSort` - iterates through a vector of integers, counts frequencies using a hashmap, and reconstructs the sorted vector.
*   **Input Data**: Random integers.
*   **Input Sizes**: 256 to 65,536 elements (Histogram), up to 1M (Random Access).
*   **Metric**: Real Time (nanoseconds).

## 3. Histogram Sort Results (at N = 65,536)

| Rank | Library | Type | Real Time (ns) | Relative Speed |
| :--- | :--- | :--- | :--- | :--- |
| **1** | **`absl::flat_hash_map`** | Flat / Open Addressing | **1,258,978** | **1.00x (Baseline)** |
| **2** | `phmap::flat_hash_map` | Flat / Open Addressing | 1,812,423 | 1.44x slower |
| **3** | `robin_hood::unordered_map` | Open Addressing | 2,344,016 | 1.86x slower |
| **4** | `std::unordered_map` | Node-based / Chaining | 3,373,203 | 2.68x slower |

## 4. Detailed Analysis (Histogram)

### The Winner: Abseil (`absl`)
**`absl::flat_hash_map`** took the top spot for the histogram workload. Its Swiss Table design provides excellent cache locality, which is crucial when iterating and updating counts frequently.

### The Standard Library (`std::unordered_map`)
*   **Performance**: Consistent slow performer.
*   **Reason**: The "node-based" approach destroys cache locality. Every access involves a potential cache miss due to pointer chasing.

### Robin Hood & Phmap
*   **`phmap`**: Based on the same Swiss Table design as Abseil, it performs very well but was slightly edged out by `absl` in this specific insert-heavy test.
*   **`robin_hood`**: A strong contender, significantly faster than `std`, but trailed the Google-derived maps in this workload.

## 5. Random Access Benchmark Results

**Methodology**:
*   **Test**: Random Access (Lookup) Speed.
*   **Procedure**: Insert N random integers, then perform lookups.
*   **Metric**: Real Time per lookup (nanoseconds).

**Results (at N = 1,048,576 / 1 Million items):**

| Rank | Library | Real Time (ns) | Relative Speed |
| :--- | :--- | :--- | :--- |
| **1** | **`phmap::flat_hash_map`** | **10.79 ns** | **1.00x (Baseline)** |
| **2** | `robin_hood::unordered_map` | 12.87 ns | 1.19x slower |
| **3** | `absl::flat_hash_map` | 13.03 ns | 1.21x slower |
| **4** | `std::unordered_map` | 27.04 ns | 2.51x slower |

**Analysis**:
*   **`phmap` Takes the Crown**: For pure random access lookups, Parallel Hashmap proved to be the fastest, clocking in at just under 11ns per lookup.
*   **Tight Competition**: `robin_hood` and `absl` are neck-and-neck, both around 13ns.
*   **Std Map Lag**: `std::unordered_map` is more than twice as slow (27ns), highlighting the cost of cache misses in large datasets.

## 6. Build Configuration Note
These results were obtained in **Release** mode.
*   In **Debug** mode, `absl` and `phmap` can be significantly slower due to the lack of inlining.
*   **Conclusion**: Always benchmark in Release mode.

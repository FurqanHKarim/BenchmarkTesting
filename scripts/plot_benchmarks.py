import json
import matplotlib.pyplot as plt
import re
import os
import argparse
import numpy as np

def parse_benchmark_name(full_name):
    # Regex to extract Map Type and Size
    # Example: BM_HistogramSort<std::unordered_map<int, int>>/256
    match = re.search(r'<(.+)>/(.+)', full_name)
    if match:
        map_type = match.group(1).replace('<int, int>', '') # Clean up for legend
        size = int(match.group(2))
        return map_type, size
    return None, None

def add_complexity_reference(ax, sizes, times, label, power=1, style='--', color='gray'):
    """
    Adds a reference line (e.g. O(n), O(n^2)) scaled to match the first data point.
    """
    if not sizes:
        return

    # Sort sizes just in case
    sizes = sorted(sizes)
    
    # Use the first point of the provided data series to anchor the reference line
    x0 = sizes[0]
    y0 = times[0]
    
    # Calculate Constant C such that y0 = C * x0^power
    C = y0 / (x0 ** power)
    
    # Generate y values for the reference line
    ref_y = [C * (x ** power) for x in sizes]
    
    ax.plot(sizes, ref_y, linestyle=style, color=color, alpha=0.7, label=f'Reference {label}', zorder=0)

def plot_benchmark(json_file, title=None, output_file=None):
    if not os.path.exists(json_file):
        print(f"File {json_file} not found. Skipping.")
        return

    with open(json_file, 'r') as f:
        try:
            data = json.load(f)
        except json.JSONDecodeError:
            print(f"Error decoding JSON from {json_file}. File might be empty or invalid.")
            return

    benchmarks = data.get('benchmarks', [])
    
    results = {}
    all_sizes = set()

    for bm in benchmarks:
        name = bm['name']
        if name.endswith('_BigO') or name.endswith('_RMS'):
            continue
            
        map_type, size = parse_benchmark_name(name)
        if map_type and size:
            if map_type not in results:
                results[map_type] = {'sizes': [], 'times': []}
            results[map_type]['sizes'].append(size)
            results[map_type]['times'].append(bm['cpu_time'])
            all_sizes.add(size)

    if not results:
        print(f"No valid benchmark data found in {json_file}.")
        return

    # Determine default output filename if not provided
    if not output_file:
        base_name = os.path.splitext(os.path.basename(json_file))[0]
        output_file = os.path.join(os.path.dirname(json_file), f"{base_name}_plot.png")
    
    # Determine default title if not provided
    if not title:
        title = f"Benchmark Results: {os.path.basename(json_file)}"

    # Plotting
    fig, ax = plt.subplots(figsize=(12, 8))
    
    markers = ['o', 's', '^', 'D', 'v', '<', '>']
    i = 0
    
    # Collect all points to determine min/max for reference lines
    min_size = min(all_sizes)
    max_size = max(all_sizes)
    sorted_all_sizes = sorted(list(all_sizes))
    
    # Find a baseline series (middle performer or just the first one) to anchor references
    # We'll use the first one in the list for simplicity, or the one with max range
    baseline_key = list(results.keys())[0]
    baseline_sizes = sorted(results[baseline_key]['sizes'])
    baseline_times = [x for _, x in sorted(zip(results[baseline_key]['sizes'], results[baseline_key]['times']))]

    for map_type, data_points in results.items():
        sorted_pairs = sorted(zip(data_points['sizes'], data_points['times']))
        sizes = [p[0] for p in sorted_pairs]
        times = [p[1] for p in sorted_pairs]
        
        ax.plot(sizes, times, marker=markers[i % len(markers)], label=map_type, linewidth=2)
        i += 1

    # Add Reference Lines
    # O(1) - Constant time (useful for lookup)
    # Anchor to the smallest time at min size
    min_time = min(min(d['times']) for d in results.values())
    ax.plot([min_size, max_size], [min_time, min_time], linestyle=':', color='black', alpha=0.5, label='O(1) Ref')

    # O(n) - Linear
    add_complexity_reference(ax, sorted_all_sizes, baseline_times, "O(n)", power=1, style='--', color='gray')
    
    # O(n^2) - Quadratic (if relevant, usually for bad cases)
    # We anchor this to the LAST point to show how bad it COULD be, or first point.
    # Anchoring to first point often makes the line shoot off the graph too fast if the actual data is better.
    # Let's anchor to the baseline first point.
    # add_complexity_reference(ax, sorted_all_sizes, baseline_times, "O(n^2)", power=2, style='-.', color='darkred')

    # O(log n) - Logarithmic (often for tree maps)
    # y = C * log(x)
    # This doesn't follow the power law y = C*x^k, so we do it manually if needed.
    # For log-log plot, log(n) is a curve that flattens out.
    
    ax.set_title(title, fontsize=16)
    ax.set_xlabel('Input Size (N)', fontsize=12)
    ax.set_ylabel('CPU Time (ns)', fontsize=12)
    ax.set_xscale('log')
    ax.set_yscale('log')
    ax.grid(True, which="both", ls="-", alpha=0.5)
    ax.legend(fontsize=10, loc='upper left', bbox_to_anchor=(1, 1))
    plt.tight_layout()
    
    plt.savefig(output_file)
    print(f"Saved plot to {output_file}")
    plt.close()

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Plot benchmark results from JSON file.")
    parser.add_argument("json_file", help="Path to the JSON benchmark results file")
    parser.add_argument("-o", "--output", help="Path to output PNG file (optional)")
    parser.add_argument("-t", "--title", help="Chart title (optional)")
    
    args = parser.parse_args()
    
    plot_benchmark(args.json_file, args.title, args.output)

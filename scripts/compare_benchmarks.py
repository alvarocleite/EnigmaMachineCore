#!/usr/bin/env python3
import json
import sys
import os

THRESHOLD = 5.0  # 5% threshold

def load_benchmarks(file_path):
    if not os.path.exists(file_path):
        print(f"Error: {file_path} not found.")
        return None
    with open(file_path, 'r') as f:
        data = json.load(f)
        return {b['name']: b for b in data['benchmarks']}

def compare(baseline, current):
    regressions = []
    
    for name, curr_data in current.items():
        if name not in baseline:
            print(f"Skipping {name} (not in baseline).")
            continue
        
        base_data = baseline[name]
        
        # Determine comparison metric: bytes_per_second (higher is better)
        # or cpu_time (lower is better)
        if 'bytes_per_second' in curr_data and 'bytes_per_second' in base_data:
            base_bps = base_data['bytes_per_second']
            curr_bps = curr_data['bytes_per_second']
            # Percentage change: negative is bad (regression)
            diff_percent = ((curr_bps - base_bps) / base_bps) * 100
            
            print(f"{name:50}: {base_bps/1024/1024:7.2f} MiB/s -> {curr_bps/1024/1024:7.2f} MiB/s ({diff_percent:+.2f}%)")
            
            if diff_percent < -THRESHOLD:
                regressions.append(f"{name}: Throughput decreased by {-diff_percent:.2f}% (Threshold: {THRESHOLD}%)")
        else:
            # Fallback to cpu_time
            base_cpu = base_data['cpu_time']
            curr_cpu = curr_data['cpu_time']
            # Percentage change: positive is bad (regression)
            diff_percent = ((curr_cpu - base_cpu) / base_cpu) * 100
            
            print(f"{name:50}: {base_cpu:10.2f} ns -> {curr_cpu:10.2f} ns ({diff_percent:+.2f}%)")
            
            if diff_percent > THRESHOLD:
                regressions.append(f"{name}: Latency increased by {diff_percent:.2f}% (Threshold: {THRESHOLD}%)")
                
    return regressions

def main():
    if len(sys.argv) < 3:
        print("Usage: compare_benchmarks.py <baseline.json> <current.json>")
        sys.exit(1)
        
    baseline_path = sys.argv[1]
    current_path = sys.argv[2]
    
    baseline = load_benchmarks(baseline_path)
    current = load_benchmarks(current_path)
    
    if not baseline or not current:
        sys.exit(1)
        
    print("Benchmark Comparison (PR vs. Baseline v1.0)")
    print("-" * 80)
    
    regressions = compare(baseline, current)
    
    print("-" * 80)
    if regressions:
        print("PERFORMANCE REGRESSIONS DETECTED:")
        for r in regressions:
            print(f"  FAILED: {r}")
        sys.exit(1)
    else:
        print("No regressions detected. Performance is within acceptable thresholds.")
        sys.exit(0)

if __name__ == "__main__":
    main()

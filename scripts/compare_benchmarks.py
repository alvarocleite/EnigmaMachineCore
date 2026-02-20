#!/usr/bin/env python3
import json
import sys
import os
import argparse
from typing import Dict, List, Optional

# Constants
THRESHOLD_PERCENT = 5.0
MIB_FACTOR = 1024 * 1024

class BenchmarkComparison:
    """Represents the comparison between a single baseline and current benchmark entry."""
    
    def __init__(self, name: str, base_entry: Dict, curr_entry: Dict):
        self.name = name
        self.is_throughput = 'bytes_per_second' in curr_entry and 'bytes_per_second' in base_entry
        
        if self.is_throughput:
            self.base_val = base_entry['bytes_per_second']
            self.curr_val = curr_entry['bytes_per_second']
            # Percentage change: negative is bad for throughput
            self.diff_percent = ((self.curr_val - self.base_val) / self.base_val) * 100
            self.is_regression = self.diff_percent < -THRESHOLD_PERCENT
        else:
            self.base_val = base_entry['cpu_time']
            self.curr_val = curr_entry['cpu_time']
            # Percentage change: positive is bad for latency
            self.diff_percent = ((self.curr_val - self.base_val) / self.base_val) * 100
            self.is_regression = self.diff_percent > THRESHOLD_PERCENT
            
        self.status_icon = self._get_status_icon()

    def _get_status_icon(self) -> str:
        if abs(self.diff_percent) <= THRESHOLD_PERCENT:
            return "✅"
        return "❌" if self.is_regression else "⚠️"

    def format_value(self, value: float) -> str:
        return f"{value/MIB_FACTOR:.2f} MiB/s" if self.is_throughput else f"{value:.1f} ns"

    def get_regression_message(self) -> str:
        metric = "Throughput" if self.is_throughput else "Latency"
        direction = "decreased" if self.is_throughput else "increased"
        return f"{self.name}: {metric} {direction} by {abs(self.diff_percent):.2f}% (Threshold: {THRESHOLD_PERCENT}%)"

def load_benchmark_map(file_path: str) -> Dict[str, Dict]:
    """Loads a Google Benchmark JSON file and returns a mapping of name to entry."""
    if not os.path.exists(file_path):
        raise FileNotFoundError(f"Benchmark file not found: {file_path}")
        
    with open(file_path, 'r') as f:
        data = json.load(f)
        return {entry['name']: entry for entry in data.get('benchmarks', [])}

def generate_comparisons(baseline_map: Dict, current_map: Dict) -> List[BenchmarkComparison]:
    """Compares current results against baseline and returns a list of comparisons."""
    comparisons = []
    for name, curr_entry in current_map.items():
        if name not in baseline_map:
            print(f"Skipping {name} (not in baseline).")
            continue
            
        comparisons.append(BenchmarkComparison(name, baseline_map[name], curr_entry))
    return comparisons

def print_console_report(comparisons: List[BenchmarkComparison]):
    """Prints a human-readable comparison report to the console."""
    header = f"{'Benchmark':50} | {'Baseline':>15} | {'Current':>15} | {'Change':>10}"
    print(header)
    print("-" * len(header))
    
    for comp in comparisons:
        base_str = comp.format_value(comp.base_val)
        curr_str = comp.format_value(comp.curr_val)
        print(f"{comp.name:50} | {base_str:>15} | {curr_str:>15} | {comp.diff_percent:>+9.2f}% {comp.status_icon}")

def write_github_summary(comparisons: List[BenchmarkComparison], summary_path: str):
    """Appends a Markdown formatted comparison table to the GitHub Step Summary."""
    with open(summary_path, 'a') as f:
        f.write("### 🚀 Benchmark Comparison (PR vs CI Baseline)\n\n")
        f.write("| Benchmark | Baseline | Current | Change |\n")
        f.write("| :--- | :--- | :--- | :--- |\n")
        
        for comp in comparisons:
            base_str = comp.format_value(comp.base_val)
            curr_str = comp.format_value(comp.curr_val)
            f.write(f"| {comp.name} | {base_str} | {curr_str} | {comp.diff_percent:+.2f}% {comp.status_icon} |\n")
        
        regressions = [c for c in comparisons if c.is_regression]
        if regressions:
            f.write("\n#### ❌ Regressions Detected\n")
            for reg in regressions:
                f.write(f"- **{reg.get_regression_message()}**\n")
        else:
            f.write(f"\n✅ All benchmarks within acceptable threshold ({THRESHOLD_PERCENT}%).\n")

def main():
    parser = argparse.ArgumentParser(description="Enigma Performance Regression Tracker")
    parser.add_argument("baseline", help="Baseline JSON file path")
    parser.add_argument("current", help="Current results JSON file path")
    parser.add_argument("--github-summary", help="Optional: Path to GitHub Actions summary file")
    
    args = parser.parse_args()
    
    try:
        baseline_map = load_benchmark_map(args.baseline)
        current_map = load_benchmark_map(args.current)
    except Exception as e:
        print(f"FATAL: {e}")
        sys.exit(1)
        
    comparisons = generate_comparisons(baseline_map, current_map)
    
    print("\nBenchmark Comparison Report")
    print("=" * 100)
    print_console_report(comparisons)
    print("=" * 100)
    
    if args.github_summary:
        write_github_summary(comparisons, args.github_summary)
        
    regressions = [c for c in comparisons if c.is_regression]
    if regressions:
        print("\n❌ FAILED: Performance regressions detected.")
        for reg in regressions:
            print(f"  - {reg.get_regression_message()}")
        sys.exit(1)
    
    print("\n✅ SUCCESS: Performance is stable.")
    sys.exit(0)

if __name__ == "__main__":
    main()

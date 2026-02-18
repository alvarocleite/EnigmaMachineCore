# Benchmarking Guide

EnigmaMachineCore uses the **Google Benchmark** framework to provide high-resolution timing and throughput metrics for its cryptographic engine.

## Overview

The benchmarking suite is designed to measure:
*   **Initialization Overhead:** The time required to parse TOML configurations and initialize the machine.
*   **Encryption Throughput:** Characters processed per second for various message lengths.
*   **Component Latency:** Micro-benchmarks for individual Rotors and the PlugBoard.

## Prerequisites

Benchmarking requires **Google Benchmark**. The build system automatically downloads and configures it via CMake's `FetchContent` when enabled.

## How to Build and Run

### 1. Configure and Build
By default, benchmarks are **disabled** to keep the core build fast. You must explicitly enable them:

```bash
# Create a dedicated build directory for benchmarking
mkdir -p build_bench && cd build_bench

# Configure with benchmarks enabled
cmake .. -DCMAKE_BUILD_TYPE=Release -DENIGMA_BUILD_BENCHMARKS=ON

# Compile the benchmark executable
make EnigmaBenchmark
```

*Note: It is highly recommended to run benchmarks in **Release** mode for accurate results.*

### 2. Run the Benchmarks
To ensure the executable can find the necessary configuration files, you should run it from the `benchmarks/` directory within your build folder:

```bash
cd benchmarks
./EnigmaBenchmark
```

## Understanding the Output

The benchmark output provides several key metrics:

*   **Time/CPU:** The average time taken per iteration.
*   **Iterations:** How many times the code was executed to get a stable average.
*   **Items/s:** For the `KeyTransform` benchmarks, this represents characters processed per second (e.g., `10.5M/s`).

### Example Output
```text
------------------------------------------------------------------------------
Benchmark                                    Time             CPU   Iterations
------------------------------------------------------------------------------
BM_EnigmaMachine_Initialization        1518195 ns      1515302 ns          483
BM_EnigmaMachine_KeyTransform/1024       97218 ns        97066 ns         7434 items_per_second=10.5M/s
BM_Rotor_Transform                        5.09 ns         5.08 ns    100000000
BM_PlugBoard_Swap                         2.92 ns         2.91 ns    250437280
```

## Command Line Options

Google Benchmark supports several useful flags:

*   **Filter:** Run only specific benchmarks matching a regex.
    `./EnigmaBenchmark --benchmark_filter=BM_Rotor`
*   **Format:** Output results in CSV or JSON.
    `./EnigmaBenchmark --benchmark_format=json > results.json`
*   **List:** List all available benchmarks without running them.
    `./EnigmaBenchmark --benchmark_list_tests`

## Integration

### CI/CD
Benchmarks are automatically executed on every Pull Request via GitHub Actions. Results are uploaded as artifacts for performance regression analysis.

### Local Development
When optimizing the "hot path" of the engine (e.g., `Rotor::transform`), use the `BM_Rotor_Transform` benchmark to verify that your changes provide a measurable speedup without regressing other components.

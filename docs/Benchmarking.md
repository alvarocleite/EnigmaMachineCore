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

### Performance & Memory Baselines

The following reference metrics were established for the **v1.0** release to guide optimization and prevent regressions:

| Metric | Reference Value (v1.0) | Description |
| :--- | :--- | :--- |
| **Initialization Peak Heap** | ~27.7 KB | Total heap memory used during configuration loading and machine setup. |
| **Hot-Path Allocations** | 1 per character | Number of dynamic memory allocations during `keyTransform`. |
| **Peak Stack Depth** | ~450 Bytes | Maximum stack space used by the transformation call chain. |
| **Throughput (3 Rotors)** | ~7.7 MiB/s | Average encryption speed for 128 KB messages (Release Build). |
| **Rotor Transform Latency** | ~5.5 ns | Average time to process a character through a single rotor. |
| **Rotor Rotate Latency** | ~3.6 ns | Overhead of the mechanical stepping logic per rotor. |

### Standard Reference Environment (SRE)
Two official baselines are maintained to ensure accuracy across different execution contexts:

1.  **Workstation Baseline (`docs/benchmarks/baseline_v1.0.json`):**
    *   **Purpose:** High-performance reference for absolute throughput goals.
    *   **OS/CPU:** Linux / 8 x 4500 MHz (Intel/AMD)
    *   **Compiler:** GCC 15.2.1 (-O3)

2.  **CI Baseline (`docs/benchmarks/baseline_ci_v1.0.json`):**
    *   **Purpose:** Regression tracking for GitHub Actions.
    *   **Environment:** Ubuntu Latest (GitHub Hosted Runner)
    *   **Note:** This file is used by the automated CI workflow to avoid false positives caused by hardware differences between CI runners and local workstations.

### CI/CD
Benchmarks are automatically executed on every Pull Request via GitHub Actions. The current performance is compared against the **CI Baseline** (`baseline_ci_v1.0.json`). Results are uploaded as artifacts for manual review if the automated check fails.

### Local Development Workflow
When optimizing the "hot path" of the engine (e.g., `Rotor::transform`), avoid comparing your local results directly against the repository's baseline files, as differences in hardware (CPU frequency, cache, etc.) will produce misleading results.

Instead, follow this "A/B" workflow on your local machine:

1.  **Generate a Local Baseline:** Checkout the `main` branch, build in Release, and save results:
    ```bash
    ./EnigmaBenchmark --benchmark_out=base.json --benchmark_out_format=json
    ```
2.  **Benchmark your Changes:** Checkout your feature branch and repeat:
    ```bash
    ./EnigmaBenchmark --benchmark_out=new.json --benchmark_out_format=json
    ```
3.  **Compare Locally:** Use the provided script to see the relative delta:
    ```bash
    python3 scripts/compare_benchmarks.py base.json new.json
    ```

### Performance Regression Threshold
The CI environment enforces a **5% regression threshold** against the **CI Baseline**.
*   **Why 5%?** Benchmarking on shared CI runners (like GitHub Actions) is subject to "system noise" (CPU scaling, OS interrupts). A 5% buffer ensures that we only fail for genuine algorithmic regressions, not minor hardware fluctuations.
*   **Baseline Management:** If a PR significantly alters the engine's architecture in a way that intentionally changes the performance profile, the official CI baseline may need to be updated.

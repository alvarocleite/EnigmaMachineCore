# EnigmaMachineCore Roadmap

This roadmap outlines the evolution of `EnigmaMachineCore` from a C++ library into a universal cryptographic backend capable of running on everything from high-end Desktop environments to constrained RTOS (Zephyr), Mobile (Android), and Web (WASM).

## Vision
To provide a high-performance, zero-overhead, and platform-agnostic Enigma cipher core that serves as a reference implementation for modern C++20 cryptographic engineering.

## Current State (v0.x)
- [x] Core cryptographic logic (Rotors, Plugboard, Reflector).
- [x] Modern C++20 architecture with Dependency Injection (DI).
- [x] Basic TOML configuration support via `toml11`.
- [x] Automated CI/CD for Linux, Windows, and macOS.

---

## Phase 1: Foundation & Performance Baseline (v1.0)
**Goal:** Stabilize the API and establish performance/memory metrics.

- [x] **Performance Benchmarking:**
    - Integrate `Google Benchmark` to measure encryption throughput (chars/sec).
    - Establish a baseline for latency and memory usage.
- [x] **CMake Modernization & Distribution:**
    - Support `BUILD_SHARED_LIBS` for Desktop/Android integration.
    - Implement `install()` rules and export `EnigmaMachineCoreConfig.cmake` for `find_package`.
- [x] **Memory Profiling:**
    - Use `Valgrind`/`Sanitizers` to ensure zero leaks in the core library.
- [x] **Architecture Decision Records (ADRs):**
    - Document core design choices (e.g., Signal path, DI strategy) in [docs/adr/ADRs.md](docs/adr/ADRs.md).

## Phase 2: Universal Portability & PAL (Platform Abstraction Layer)
**Goal:** Decouple from the OS and C++ Runtime (Exceptions/Filesystem).

- [ ] **Zero-Overhead Refactor (Static DI):**
    - Move from virtual interfaces to **C++20 Concepts and Templates** in the hot signal path to eliminate vtable overhead.
- [ ] **Error Handling & Exception-Free Core:**
    - Transition from `throw` to `std::expected` (or `tl::expected`) for all public APIs.
    - Enable `-fno-exceptions` support for embedded and WASM targets.
- [x] **Logging & IO Abstraction (`ILogger` / `PAL`):**
    - Remove `std::cout`/`std::cerr`.
    - Implement a `PAL` (Platform Abstraction Layer) for Logging (Logcat, UART, Console).
- [ ] **POD Configuration & No-std Prep:**
    - Define `EnigmaMachineData` structs for direct initialization.
    - Abstract `std::iostream` and `std::filesystem` out of the Core logic.

## Phase 3: Interoperability & Ecosystem Integration
**Goal:** Expose the core to all major application environments via a stable ABI.

- [ ] **Stable C-API (`enigma_core_c.h`):**
    - Define a pure C interface to ensure compatibility with Rust, Swift, and Go.
- [ ] **Android & WASM Track:**
    - **Android:** JNI/NDK bridge and `AAR` packaging.
    - **WASM:** `Emscripten` build pipeline with `Embind` for JS/TS bindings.
- [ ] **High-Level Bindings:**
    - Implement `pybind11` for Python support.
- [ ] **Cross-Compilation CI:**
    - Add GitHub Actions for `wasm32-unknown-unknown` and `arm-none-eabi` (Cortex-M).

## Phase 4: Constrained Embedded & Hardening
**Goal:** Hardware-ready execution for Zephyr.

- [ ] **Zephyr RTOS Module:**
    - Add `zephyr/module.yml` and `Kconfig` support.
- [ ] **Build-Time Code Generation:**
    - Python script to convert `.toml` configs into `const` C++ structs (Flash-friendly).
- [ ] **Security & Hardening:**
    - Integrate `LLVM libFuzzer` for the signal path and config loaders.
    - Investigate constant-time operations for sensitive logic.
- [ ] **Zero-Allocation Mode:**
    - Ensure the core can run without `malloc`/`new` after initialization.

## Phase 5: Industrial Linux & System Integration (Yocto)
**Goal:** Provide first-class support for Embedded Linux distributions.

- [ ] **BitBake Recipe Development:**
    - Create a `meta-enigma` layer with recipes for the Core library and CLI tool.
- [ ] **Package Configuration (pkg-config):**
    - Generate `.pc` files via CMake to support standard Linux linking conventions.
- [ ] **Runtime Optimization:**
    - Validate performance and thermal impact on low-power ARM SoCs (e.g., i.MX6/8, Raspberry Pi).
- [ ] **System-Wide Installation:**
    - Ensure robust support for `/usr/lib` and `/usr/include` standard paths.

---

## Target Matrix & Status

| Target | Build System | Config Strategy | Status |
| :--- | :--- | :--- | :--- |
| **CLI (Linux/Win/Mac)** | CMake | TOML (Runtime) | ✅ Basic |
| **Web (WASM)** | Emscripten | Memory/Embedded | 🏗️ Planned |
| **Android (Kotlin/JNI)**| Gradle/CMake | AAssetManager | 🏗️ Planned |
| **Embedded (Zephyr)** | West/CMake | Static (Flash) | 🏗️ Planned |
| **Industrial Linux (Yocto)** | BitBake/CMake | TOML (Runtime) | 🏗️ Planned |
| **Python** | pybind11 | Memory/String | 🏗️ Planned |

## Success Metrics
- **Portability:** Core logic compiles with `-fno-exceptions` and `-fno-rtti`.
- **Performance:** Zero performance regression between vtable-based DI and Template-based DI.
- **Size:** Embedded binary footprint (Core only) < 50KB.

## Non-Goals
- We will NOT implement UI components (GUI/Mobile Screens) in this repository.
- We will NOT support legacy C++ standards (< C++20).

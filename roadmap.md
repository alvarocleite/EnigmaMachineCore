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

## Phase 1: Foundation & Performance Baseline (v0.1.0)
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
- [x] **Historical Assets & Documentation:**
    - Integrated full set of historical Enigma I rotors (I-V) and reflectors (A-C).
    - Documented wiring sources and technical specifications in [docs/Historical_Data.md](docs/Historical_Data.md).
- [x] **Technical Documentation & Visualization:**
    - Created PlantUML diagrams for RotorBox assembly, Plugboard structure, and Signal Flow.
    - Integrated visualizations into the source code documentation (Doxygen).
- [ ] **Verified Code Coverage:**
    - Integrate `gcov/lcov` and a service like **Codecov** to ensure the core signal path has 100% branch coverage.

## Phase 2: Universal Portability & "No-Filesystem" Mode (v0.2.0)
**Goal:** Decouple from the OS and enable initialization without disk I/O.

- [ ] **Static Configuration Support (Critical for Zephyr):**
    - Expose `EnigmaMachine` constructors that accept `EnigmaMachineConfig` (DTO) directly.
    - Ensure all core components can be initialized from Plain Old Data (POD) structures.
- [ ] **State Serialization (Save/Restore):**
    - Implement a standardized way to export and restore the current mutable state (rotor positions, etc.) into a minimal binary buffer.
- [ ] **Dynamic Alphabet & Size Support:**
    - Transition `TRANSFORMER_SIZE` from a constant to a template parameter for compile-time optimization of arbitrary alphabet sizes (e.g., 26, 29, 36, 256).
- [ ] **Correctness Verification (Property-Based Testing):**
    - Integrate `RapidCheck` to verify mathematical reciprocity (`Encrypt(Encrypt(x)) == x`) and state transitions across thousands of random configurations.
- [ ] **Historical Model Factory Methods:**
    - Provide static factory methods (e.g., `createEnigmaI()`, `createM3()`) to instantiate standard historical models with a single call.
- [ ] **Exception-Free Core & Modern Error Handling:**
    - Transition from `throw` to `std::expected` (or `tl::expected`) for all public APIs.
    - Enable `-fno-exceptions` support for embedded and WASM targets.
- [x] **Logging & IO Abstraction (`ILogger` / `PAL`):**
    - Remove `std::cout`/`std::cerr`.
    - Implement a `PAL` (Platform Abstraction Layer) for Logging (Logcat, UART, Console).
- [ ] **Zero-Overhead Refactor (Static DI):**
    - Move from virtual interfaces to **C++20 Concepts and Templates** in the hot signal path to eliminate vtable overhead.

## Phase 3: Interoperability & Stable C-ABI (v0.3.0)
**Goal:** Expose the core to all major application environments via a universal translator.

- [ ] **Stable C-Interface (`enigma_core_c.h`):**
    - Create an `extern "C"` wrapper for the `EnigmaMachine` public API.
    - Support opaque pointers for instance management (FFI-friendly).
- [ ] **ABI Stability Tracking:**
    - Integrate tools like `libabigail` into CI to ensure updates don't unintentionally break the binary interface for library consumers.
- [ ] **High-Level Convenience API:**
    - Add `encrypt(std::string_view)` and `decrypt(std::string_view)` wrappers that handle character-to-index mapping and filtering automatically.
- [ ] **Package Manager Integration:**
    - Develop and maintain official recipes for **Conan** and **vcpkg** to simplify library distribution and consumption.
- [ ] **Platform-Specific "Starter Kits":**
    - Provide minimal `samples/` for Zephyr, Android (JNI), and WASM to reduce integration friction.
- [ ] **Android & WASM Track:**
    - **Android:** JNI/NDK bridge using the C-API.
    - **WASM:** `Emscripten` build pipeline with `Embind` or direct C-exports.
- [ ] **High-Level Bindings:**
    - Implement `pybind11` for Python support (or use `ctypes` over the C-API).
- [ ] **Cross-Compilation CI:**
    - Add GitHub Actions for `wasm32-unknown-unknown` and `arm-none-eabi` (Cortex-M).

## Phase 4: Constrained Embedded & Heap-Agnostic Core (v0.4.0)
**Goal:** Eliminate dynamic memory dependency and harden the engine.

- [ ] **Heap-Agnostic Core Refactor:**
    - Replace `std::vector` and `std::unique_ptr` with `std::array` and static/inline storage where `MAX_ROTORS` is known.
    - Ensure the engine can run entirely on the stack or in pre-allocated static memory.
- [ ] **Zero-Allocation Mode:**
    - Guarantee zero `new`/`malloc` calls after the initialization phase.
- [ ] **Binary Size Profiling & Budgeting:**
    - Implement CI checks using `bloaty` to monitor `.text`/`.data` usage on `arm-none-eabi`, maintaining a <50KB footprint.
- [ ] **Zephyr RTOS Module:**
    - Add `zephyr/module.yml` and `Kconfig` support.
- [ ] **Build-Time Code Generation:**
    - Python script to convert `.toml` configs into `const` C++ structs (Flash-friendly).
- [ ] **Security & Hardening:**
    - Integrate `LLVM libFuzzer` for the signal path and config loaders.
    - Investigate constant-time operations for sensitive logic.

## Phase 5: Industrial Linux & System Integration (Yocto)
**Goal:** Provide first-class support for Embedded Linux distributions.

- [ ] **BitBake Recipe Development:**
    - Create a `meta-enigma` layer with recipes for the Core library and CLI tool.
- [ ] **Package Configuration (pkg-config):**
    - Generate `.pc` files via CMake to support standard Linux linking conventions.
- [ ] **SIMD/Vectorization for Block Processing:**
    - Implement SIMD-accelerated variants of `processBuffer` for x86_64 (AVX2) and AArch64 (NEON).
- [ ] **Runtime Optimization:**
    - Validate performance and thermal impact on low-power ARM SoCs (e.g., i.MX6/8, Raspberry Pi).
- [ ] **System-Wide Installation:**
    - Ensure robust support for `/usr/lib` and `/usr/include` standard paths.
- [ ] **Professional Narrative Documentation:**
    - Integrate **Sphinx, Breathe, and Exhale** to generate a searchable, hosted website combining narrative tutorials with extracted API docs.

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
- **Memory:** Zero heap allocations in the hot encryption path.
- **Correctness:** 100% pass rate on property-based reciprocity tests.

## Non-Goals
- We will NOT implement UI components (GUI/Mobile Screens) in this repository.
- We will NOT support legacy C++ standards (< C++20).

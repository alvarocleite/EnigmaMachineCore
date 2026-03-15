# EnigmaMachineCore Roadmap

This roadmap outlines the evolution of `EnigmaMachineCore` from a C++ library into a universal cryptographic backend capable of running on everything from high-end Desktop environments to constrained RTOS (Zephyr), Mobile (Android), and Web (WASM).

## Vision
To provide a high-performance, zero-overhead, and platform-agnostic Enigma cipher core that serves as a reference implementation for modern C++20 cryptographic engineering.

## Current State (v0.1.0)
**Status:** Phase 1 implementation complete; v0.1.0 release ready.

- [x] Core cryptographic logic (Rotors, Plugboard, Reflector).
- [x] Modern C++20 architecture with Dependency Injection (DI).
- [x] Basic TOML configuration support via `toml11`.
- [x] Automated CI/CD for Linux, Windows, and macOS (11 comprehensive workflows).
- [x] Comprehensive benchmarking suite with baseline established.
- [x] Full memory profiling (Valgrind + 4 sanitizers) in CI.
- [x] Historical Enigma I reference models (Rotors I-V, Reflectors A-C).

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
- [x] **Verified Code Coverage:**
    - Integrate `gcov/lcov` into CMake build for coverage reporting.
    - Set up **Codecov** or similar service for automated branch coverage tracking.
    - Enforce minimum coverage thresholds in CI (goal: 95% for signal path, 75% overall).
- [x] **Property-Based Testing Foundation:**
    - Integrate `RapidCheck` for reciprocity verification (`Encrypt(Encrypt(x)) == x`).
    - Add 10+ randomized configuration tests.

## Phase 2: Universal Portability & "No-Filesystem" Mode (v0.2.0)
**Goal:** Decouple from the OS and enable initialization without disk I/O.

**Sequencing Note:** Phase 2 is split into 3 sub-phases
- **Phase 2a:** Exception-Free Core (Constructors -> Static Factories)
- **Phase 2b:** Platform Abstraction Layer (PAL) & Logging
- **Phase 2c:** Zero-Overhead Static DI (Templates & Concepts)

### Phase 2a: Exception-Free Core & Factory Methods

- [ ] **Constructor Refactoring to Static Factories:**
    - Move all fallible initialization logic from constructors to static factory methods.
    - Create `EnigmaMachine::create()` -> `Result<EnigmaMachine>` (using `std::expected`).
    - Keep default constructor for simple in-memory initialization.
- [ ] **Result Type Integration:**
    - Adopt `std::expected<T, Error>` for all public APIs that can fail.
    - Define a minimal `EnigmaError` enum with clear error categories.
- [ ] **POD Configuration DTOs:**
    - Create `EnigmaMachineData`, `RotorData`, `ReflectorData` structs (Plain Old Data).
    - Ensure all core components can initialize from these POD structures.
    - Expose `EnigmaMachine` constructors that accept `EnigmaMachineConfig` (DTO) directly.
- [ ] **Verify Exception-Free Compilation:**
    - Add CI job: compile core with `-fno-exceptions` and `-fno-rtti`.

### Phase 2b: Platform Abstraction Layer (PAL) & Logging

- [x] **Logging & IO Abstraction (`ILogger` / `PAL`):**
    - Remove `std::cout`/`std::cerr`.
    - Implement a `PAL` (Platform Abstraction Layer) for Logging (Logcat, UART, Console).
- [ ] **Logging Abstraction (ILogger):**
    - Ensure `ILogger` is the only runtime logging abstraction.
    - Provide `StandardOutputLogger` for CLI and `NullLogger` as default.
- [ ] **Configuration Decoupling:**
    - Decouple TOML parsing from `EnigmaMachine` (move to CLI only).
    - Ensure core accepts pre-parsed config DTOs or binary buffers.

### Phase 2c: Zero-Overhead Refactor (Static DI) & Additional Features

- [ ] **Static Polymorphism (C++20 Concepts):**
- [ ] **State Serialization (Save/Restore):**
    - **Format Specification (CRITICAL):** Define immutable binary serialization format for state snapshots (see Phase 2 planning notes).
    - Implement methods to export/restore rotor positions and plugboard mappings.
    - Ensure format supports version evolution (backward compatibility).
- [ ] **Dynamic Alphabet & Size Support:**
    - Transition `TRANSFORMER_SIZE` from constant to template parameter.
    - Enable compile-time optimization for arbitrary alphabets (26, 29, 36, 256 chars).
- [ ] **Correctness Verification (Property-Based Testing):**
    - Integrate `RapidCheck` to verify reciprocity across thousands of random configs.
- [ ] **Historical Model Factory Methods:**
    - Provide static factory methods: `createEnigmaI()`, `createM3()`, `createM4()`, etc.
    - Load from built-in wiring tables (no file I/O required).

## Phase 3: Interoperability & Stable C-ABI (v0.3.0)
**Goal:** Expose the core to all major application environments via a universal translator.

### Phase 3a: Cross-Compilation Foundation & WASM

- [ ] **Cross-Compilation CI:**
    - Add GitHub Actions for `wasm32-unknown-unknown` (Emscripten).
    - Add GitHub Actions for `arm-none-eabi` (Cortex-M / Zephyr).
    - Validate core compiles and tests pass on both targets.
- [ ] **WASM Build Pipeline:**
    - Create Emscripten build configuration.
    - Generate `enigma_core.wasm` with Embind or raw C-exports.
    - Provide simple JavaScript wrapper example.

### Phase 3b: C-Interface & Android

- [ ] **Stable C-Interface (`enigma_core_c.h`):**
    - Create an `extern "C"` wrapper for the `EnigmaMachine` public API.
    - Support opaque pointers for instance management (FFI-friendly).
- [ ] **ABI Stability Tracking:**
    - Integrate tools like `libabigail` into CI to ensure updates don't unintentionally break the binary interface for library consumers.
- [ ] **Android (JNI/NDK Bridge):**
    - Implement JNI wrapper for Kotlin/Java interop.
    - Create example Android app demonstrating library usage.
- [ ] **High-Level Convenience API:**
    - Add `encrypt(std::string_view)` and `decrypt(std::string_view)` wrappers that handle character-to-index mapping and filtering automatically.
- [ ] **Package Manager Integration:**
    - Develop and maintain official recipes for **Conan** and **vcpkg** to simplify library distribution and consumption.

### Phase 3c: Python & Additional Platforms

- [ ] **High-Level Bindings (Python):**
    - **Decision Point:** Implement `pybind11` vs `ctypes` .
    - Provide PyPI package (`enigma-core`).
    - Include example Python encryption/decryption scripts.
- [ ] **Platform-Specific "Starter Kits":**
    - Provide minimal `samples/` for Zephyr, Android (JNI), WASM, and Python to reduce integration friction.

## Phase 4: Constrained Embedded & Heap-Agnostic Core (v0.4.0)
**Goal:** Eliminate dynamic memory dependency and harden the engine for resource-constrained targets (Zephyr RTOS, microcontrollers).

- [ ] **Heap-Agnostic Core Refactor:**
    - Minimize or eliminate all dynamic memory usage in the core library.
    - Use fixed-size buffers and `std::array` where possible.
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

## Phase 5: Industrial Linux & System Integration - Yocto (v0.5.0)
**Goal:** Provide first-class support for Embedded Linux distributions.

- [ ] **BitBake Recipe Development:**
    - Create a `meta-enigma` layer with recipes for the Core library and CLI tool.
- [ ] **Package Configuration (pkg-config):**
    - Generate `.pc` files via CMake to support standard Linux linking conventions.
- [ ] **SIMD/Vectorization for Block Processing:**
    - Implement SIMD-accelerated variants of `processBuffer` for x86_64 (AVX2) and AArch64 (NEON).
- [ ] **System-Wide Installation:**
    - Ensure robust support for `/usr/lib` and `/usr/include` standard paths.
- [ ] **Professional Narrative Documentation:**
    - Integrate **Sphinx, Breathe, and Exhale** to generate a searchable, hosted website combining narrative tutorials with extracted API docs.

---

## Target Matrix & Status

| Target | Build System | Config Strategy | Priority | Phase | Status |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **CLI (Linux/Win/Mac)** | CMake | TOML (Runtime) | High | ✅ Phase 1 | ✅ Complete |
| **Web (WASM)** | Emscripten + CMake | Memory Buffer | High | Phase 3a | 🏗️ Planned |
| **Android (Kotlin/JNI)**| Gradle/CMake | AAssetManager | High | Phase 3b | 🏗️ Planned |
| **Python** | pybind11/ctypes | Memory/String | Medium | Phase 3c | 🏗️ Planned |
| **Embedded (Zephyr RTOS)** | West/CMake | Static Config (POD) | High | Phase 4 | 🏗️ Planned |
| **Industrial Linux - Yocto** | BitBake/CMake | TOML (Runtime) | Low | Phase 5 | 📋 Future |

## Success Metrics & Verification

- **Portability:** Core logic compiles with `-fno-exceptions` and `-fno-rtti`
- **Performance:** Zero performance regression between vtable-based DI and Template-based DI
- **Size:** Embedded binary footprint (Core only) < 50KB
- **Correctness:** 100% pass rate on property-based reciprocity tests

## Non-Goals

- We will NOT implement UI components (GUI/Mobile Screens) in this repository.
- We will NOT support legacy C++ standards (< C++20).

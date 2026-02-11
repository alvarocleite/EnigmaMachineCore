# EnigmaMachineCore Roadmap

This roadmap outlines the evolution of `EnigmaMachineCore` from a C++ library into a universal cryptographic backend capable of running on everything from high-end Desktop environments to constrained RTOS (Zephyr) and Web (WASM).

## Phase 1: Core Architectural Foundation (The "Universal" Refactor)
**Goal:** Decouple the engine from the filesystem and the C++ Exception runtime.

- [ ] **CMake Modernization:**
    - Support `BUILD_SHARED_LIBS` for Desktop/Android integration.
    - Implement `install()` rules for headers and binaries.
    - Export `EnigmaMachineCoreConfig.cmake` for `find_package` support.
- [ ] **Logging Abstraction (`ILogger`):**
    - Remove hardcoded `std::cout`/`std::cerr`.
    - Create a log-sink interface to support Android Logcat and Embedded UART.
- [ ] **POD Configuration Structs:**
    - Define `EnigmaMachineData` (Plain Old Data) structs.
    - Modify `EnigmaMachine` to initialize from these structs rather than loading files directly.
- [ ] **Error Handling Refactor:**
    - Transition from `throw` to `Result<T>` (using `std::expected` or a lightweight polyfill).
    - Enable `-fno-exceptions` support for embedded targets.

## Phase 2: Configuration & Asset Evolution
**Goal:** Implement the "Split-Provider Pattern" to handle different resource environments.

- [ ] **Split-Provider Implementation:**
    - **Rich Provider:** Keep `toml11` as an optional module for CLI, Desktop, and Android.
    - **Memory Provider:** Create a provider for WASM/Python where assets are pre-loaded in memory.
    - **Static Provider:** Support compile-time configuration (Code Gen) for Zephyr.
- [ ] **Polymorphic Configurator (`IConfigurator`):**
    - Introduce an interface to abstract the configuration loading process (TOML, JSON, Static).
    - Implement `TOMLConfigurator` as the default runtime loader.
    - Implement `StaticConfigurator` for unit tests and memory-constrained targets.
- [ ] **Path Independence:**
    - Ensure the `ConfigLoader` handles relative asset resolution (e.g., Rotor files inside Machine config) via the `IAssetProvider` abstraction.

## Phase 3: Desktop, Mobile & Web Integration
**Goal:** Expose the core to high-level languages and web environments.

- [ ] **Python Bindings:**
    - Implement a `pybind11` wrapper to allow `import enigma_core`.
- [ ] **WebAssembly (WASM):**
    - Configure Emscripten build pipeline.
    - Use `Embind` to expose the machine to JavaScript/TypeScript.
- [ ] **Android / Kotlin:**
    - Develop a JNI (Java Native Interface) bridge.
    - Implement `AndroidAssetProvider` using NDK `AAssetManager`.
- [ ] **Desktop Distribution:**
    - Integrate `CPack` for generating `.deb`, `.msi`, and `.dmg` installers.

## Phase 4: Constrained Embedded Support (Zephyr/Yocto)
**Goal:** Zero-overhead execution on MCUs.

- [ ] **Zephyr Module Integration:**
    - Add `zephyr/module.yml` and `Kconfig` support.
- [ ] **Build-Time Code Generation:**
    - Create a Python script to convert `.toml` configs into `const` C++ structs.
    - Allow Zephyr targets to link without `toml11` or the STL `Loader`.
- [ ] **Binary Serialization:**
    - Investigate `FlatBuffers` (no-alloc mode) for zero-copy configuration loading.

## Target Matrix & Status

| Target | Build System | Config Strategy | Status |
| :--- | :--- | :--- | :--- |
| **CLI (Linux/Win/Mac)** | CMake | TOML (Runtime) | ✅ Basic |
| **Desktop (C++/Qt)** | CMake | TOML (Runtime) | 🏗️ Planned |
| **Python** | pybind11 | Memory/String | 🏗️ Planned |
| **Web (WASM)** | Emscripten | Memory/Embedded | 🏗️ Planned |
| **Android** | Gradle/JNI | AAssetManager | 🏗️ Planned |
| **Yocto (Linux)** | BitBake | TOML (Runtime) | 🏗️ Planned |
| **Zephyr (RTOS)** | Zephyr/West | Static (Compile-time) | 🏗️ Planned |

## Future Considerations
- **Fuzz Testing:** Integrate LLVM `libFuzzer` for the TOML parser and signal path.
- **Performance:** Optimize the signal path using `std::span` and SIMD where applicable.

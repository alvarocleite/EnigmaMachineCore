# ADR 001: Dependency Injection for Assets (IAssetProvider)

## Status
Accepted

## Context
The Enigma Machine Core is designed to be universal, supporting platforms from high-end Linux systems to low-end bare-metal microcontrollers and WebAssembly (WASM). Loading configuration files (TOML) directly from the filesystem (`std::ifstream`) creates a hard dependency on an OS-level filesystem, which is absent in many of our target environments (e.g., WASM, Zephyr RTOS).

## Decision
We will decouple the Enigma Engine from the physical filesystem by introducing the `IAssetProvider` interface.

1.  **Interface:** `IAssetProvider` defines a single `loadAsset(assetName)` method that returns a `std::string` containing the asset's content.
2.  **Decoupling:** The `EnigmaMachine` and its components will not perform file I/O. Instead, they will receive an `IAssetProvider` (Dependency Injection) and use it to retrieve configuration data.
3.  **Implementations:**
    *   `FileAssetProvider`: For platforms with a filesystem (CLI, Desktop).
    *   `MemoryAssetProvider` (Planned): For platforms like WASM or Embedded where assets are pre-loaded or linked into the binary.

## Consequences
*   **Portability:** The engine can now run on platforms without a filesystem by providing a custom provider.
*   **Testability:** Unit tests can use a mock provider to simulate configuration data without relying on disk files.
*   **Complexity:** Slightly increases initialization complexity as a provider must be instantiated and passed to the machine.

# ADR 007: Result Type Implementation

## Status

Accepted

## Context

ADR 003 established the decision to transition from C++ exceptions to `std::expected` for error handling in the EnigmaCore library. This ADR documents the concrete implementation of that decision.

## Decision

### Dependency: expected-lite

We adopt `martinmoene/expected-lite` as the `std::expected` backport. It is a single-file header-only library, MIT/BSL licensed, C++11-compatible, and available as a git submodule at `external/expected-lite`.

```bash
git submodule add https://github.com/martinmoene/expected-lite.git external/expected-lite
```

The submodule's `include/nonstd/expected.hpp` is added to the build's include paths. It auto-detects `std::expected` (C++23) and falls back to its own implementation. When `std::expected` is used, `nonstd::expected` becomes a namespace alias for `std::expected`. When falling back, `nonstd::expected` refers to `nonstd::expected_lite::expected`.

### Error Enum

A flat `enum class EnigmaError` defines all error codes:

```cpp
enum class EnigmaError {
    None = 0,
    FileNotFound,
    ConfigFieldMissing,
    TransformerSizeMismatch,
    ConfigCountMismatch,
    PlugBoardExceedsMaximum,
    PlugBoardCountMismatch,
    RotorWiringOutOfRange,
    RotorWiringDuplicate,
    RotorWiringNotBijective,
    PlugBoardPortOutOfRange,
    PlugBoardPortConflict,
};
```

`EnigmaError::None` serves as a sentinel for default-constructed Results where `error()` returns `None`.

### Result Type Alias

```cpp
template <typename T>
using Result = nonstd::expected<T, EnigmaError>;
```

Defined in `include/EnigmaError.hpp`, which is part of the public API and included by `EnigmaCore.hpp`.

### Error Mapping

| Throw Site | `EnigmaError` Code |
|------------|---------------------|
| `FileAssetProvider::loadAsset` | `FileNotFound` |
| `validateTransformerConfig` (size) | `TransformerSizeMismatch` |
| `validateTransformerConfig` (missing field) | `ConfigFieldMissing` |
| `validateTransformerConfig` (type/value) | `ConfigCountMismatch` |
| `load()` count check | `ConfigCountMismatch` |
| `load()` plugboard > max | `PlugBoardExceedsMaximum` |
| `load()` plugboard count mismatch | `PlugBoardCountMismatch` |
| `Rotor::initReverseLookupTable` (out of range) | `RotorWiringOutOfRange` |
| `Rotor::initReverseLookupTable` (duplicate) | `RotorWiringDuplicate` |
| `Rotor::initReverseLookupTable` (not bijective) | `RotorWiringNotBijective` |
| `PlugBoard` (port out of range) | `PlugBoardPortOutOfRange` |
| `PlugBoard` (port conflict) | `PlugBoardPortConflict` |

### API Changes

#### IAssetProvider

```cpp
// Before
virtual std::string loadAsset(std::string_view assetName) const = 0;

// After
virtual enigma::Result<std::string> loadAsset(std::string_view assetName) const = 0;
```

#### EnigmaConfigLoader

```cpp
// Before
static EnigmaMachineConfig load(...);
static RotorConfig loadRotor(...);
static ReflectorConfig loadReflector(...);

// After
static enigma::Result<EnigmaMachineConfig> load(...);
static enigma::Result<RotorConfig> loadRotor(...);
static enigma::Result<ReflectorConfig> loadReflector(...);
```

#### EnigmaMachine Constructors (Option A Bridge)

`EnigmaMachine` constructors still throw exceptions as a temporary bridge (for backward compatibility), but task 2.2 (Static Factories) has now been implemented. The new `EnigmaMachine::create()` factory methods return `enigma::Result<EnigmaMachine>` directly. See [ADR 008](008-static-factories.md) for details.

When the constructor approach is used and `EnigmaConfigLoader` returns an error, the constructor:
1. Logs the error via `ILogger::log(LogLevel::Error, ...)` if a logger is available.
2. Throws `std::runtime_error` with a brief message.

```cpp
auto configResult = EnigmaConfigLoader::load(provider, FileName(fileName), AssetPath(assetPath));
if (!configResult) {
    logEnigmaError(logger, "Failed to load Enigma configuration", configResult.error());
    throw std::runtime_error("Failed to load Enigma configuration");
}
```

This keeps the consumer API unchanged (constructors still throw) while the internal loading layer uses `Result`.

### Internal Components

`Rotor`, `RotorBox`, and `PlugBoard` constructors **still throw exceptions** internally. Validation of POD data (duplicate wiring, port conflicts) is not yet handled via `Result`. This could be addressed in a future iteration.

ADR 006 explicitly noted: *"POD structs cannot enforce invariants at construction time. Invalid data must be validated separately."* The factory methods (task 2.2) provide a validation layer for configuration loading, but internal component validation still uses exceptions.

### Error Context

Error context (human-readable messages) is preserved via the existing `ILogger::log()` interface. When an operation fails, the error code is logged with a descriptive string prefix. Callers using `ILogger` receive error context without requiring exception strings.

## Consequences

### Positive

- `IAssetProvider` and `EnigmaConfigLoader` are now fully exception-free at the interface level.
- Error codes enable programmatic error handling in embedded/WASM consumers.
- Clear error categorization via the enum enables granular error handling.
- Logger-based context preserves diagnostic information for CLI/Desktop.

### Negative

- Constructor bridge (Option A) is a temporary layer. Task 2.2 replaces this with factory methods.
- Internal components (`Rotor`, `PlugBoard`) still throw, limiting no-exception builds until task 2.2.
- API of `IAssetProvider` changed — all implementations must update `loadAsset` return type.
- 9 test files updated to use `Result<T>` return value checks. Internal-component tests (`Rotor`, `PlugBoard`) retain `EXPECT_THROW` for validation that still throws internally.

## Implementation Notes

### File Locations

- `external/expected-lite/` — git submodule
- `include/EnigmaError.hpp` — `EnigmaError` enum + `Result<T>` alias (public)
- `include/EnigmaCore.hpp` — includes `EnigmaError.hpp`
- `EnigmaMachine/include/IAssetProvider.hpp` — updated `loadAsset` signature
- `EnigmaMachine/include/FileAssetProvider.hpp` — updated declaration
- `EnigmaMachine/include/EnigmaConfigLoader.hpp` — updated return types
- `EnigmaMachine/src/FileAssetProvider.cpp` — returns `unexpected(FileNotFound)`
- `EnigmaMachine/src/EnigmaConfigLoader.cpp` — all `throw` → `unexpected(code)`
- `EnigmaMachine/src/EnigmaMachine.cpp` — Option A bridge in constructors

### CMake Changes

- `external/expected-lite/include` added to `INCLUDE_DIRS`
- `include/EnigmaError.hpp` added to `PUBLIC_HEADERS`

## Related Decisions

- [ADR 003: Error Handling Strategy](003-error-handling-strategy.md) — Supersedes generic description with this concrete implementation
- [ADR 006: POD Configuration DTOs](006-POD-DTOs.md) — POD constructors defer validation to factories

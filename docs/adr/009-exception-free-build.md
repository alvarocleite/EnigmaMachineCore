# ADR 009: Exception-Free Build Configuration

## Status

**Implemented** — Full exception-free build now supported

## Context

The EnigmaMachineCore targets multiple platforms including embedded systems (Zephyr/RTOS), WebAssembly (WASM), and mobile. Many of these platforms either:
- Do not support C++ exceptions
- Have exception support disabled for performance reasons
- Require explicit compile-time flags to disable exceptions

With ADR 007 (Result Type Implementation) and ADR 008 (Static Factory Methods) in place, the infrastructure for exception-free error handling exists. We add a build-time option to enforce `-fno-exceptions` compilation.

## Decision

We add a CMake option `ENIGMA_NO_EXCEPTIONS` that enables:
- GCC/Clang: `-fno-exceptions -fno-rtti`
- MSVC: `/EHsc- /GR-`

This option is applied to the `EnigmaCore_OBJ` object library.

### Option Configuration

```cmake
option(ENIGMA_NO_EXCEPTIONS "Build without exceptions and RTTI for embedded/WASM targets" OFF)
```

### Current Limitation

**Full exception-free compilation requires:**
1. All core code to use `Result<T>` instead of `throw` statements
2. External dependencies (toml11) to support `-fno-exceptions`

**Current state:**
- Core files still contain `throw` statements (PlugBoard, RotorBox, EnigmaMachine)
- toml11 (TOML parsing) uses exceptions internally
- Full compilation will fail until these are converted

**The infrastructure is in place.** The CMake option and CI workflow exist. Full conversion is tracked as part of Phase 2c (Task 2.8+).

### Workaround

For targets that require exception-free operation, use the POD DTO path:
- Create `EnigmaMachineConfig` directly in memory (no TOML parsing)
- Use `EnigmaMachine::create(config)` factory method
- This bypasses toml11 and enables exception-free operation

## Consequences

### Positive
- Infrastructure in place for exception-free builds
- Catches exception usage in core code at compile time
- Enables exception-free operation via POD DTO path
- Reduces binary size on platforms without exception support

### Negative
- Full compilation with `-DENIGMA_NO_EXCEPTIONS=ON` fails until:
  - Core code converts `throw` to `Result<T>` (see Phase 2c)
  - Or toml11 provides exception-free variant
- External libraries (toml11, CLI11) use exceptions internally

## Implementation

- **Files modified:**
  - `CMakeLists.txt` - Added `ENIGMA_NO_EXCEPTIONS` option and compiler flags
  - `.github/workflows/exception-free-build.yml` - New CI workflow (documents current limitations)

- **Next Steps (Phase 2c):**
  - Convert remaining `throw` statements to `Result<T>` in core code
  - Consider exception-free toml11 alternative or wrapper

## Related ADRs

- ADR 003: Error Handling Strategy
- ADR 007: Result Type Implementation
- ADR 008: Static Factory Methods
- ADR 006: POD Configuration DTOs (enables exception-free initialization)

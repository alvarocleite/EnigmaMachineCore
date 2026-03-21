# ADR 006: Plain Old Data (POD) Configuration DTOs

## Status

Accepted

## Context

The EnigmaMachineCore is designed to run on diverse platforms, from high-end Linux desktops to constrained embedded systems (Zephyr RTOS, bare-metal) and WebAssembly (WASM). These constrained environments often lack:

- Dynamic memory allocation (`malloc`/`new`)
- Standard library containers (`std::vector`, `std::string`)
- Exception handling (`-fno-exceptions`)

At the time this ADR was written, `EnigmaMachineConfig` and related structs use non-POD types:

```cpp
struct EnigmaMachineConfig {
    int rotorCount;
    std::vector<AlphabetIndex> rotorPositions;  // Requires heap allocation
    std::vector<RotorConfig> rotors;             // Requires heap allocation
    ReflectorConfig reflector;
    std::array<PlugBoardPair, PLUGBOARD_MAX_PAIRS> plugBoardPairs;
};
```

This prevents initialization of the Enigma machine from memory buffers or stack-allocated configurations.

## Decision

We will create a parallel hierarchy of POD (Plain Old Data) configuration structures that contain only primitive types:

```cpp
#include <array>
#include "EnigmaConfig.hpp"  // For TRANSFORMER_SIZE, MAX_ROTORS, MAX_PLUGBOARD_PAIRS
#include "EnigmaTypes.hpp"   // For AlphabetIndex

namespace enigma {

/**
 * @brief Pure POD struct for rotor configuration.
 * No constructors, no heap allocation, trivially copyable.
 */
struct RotorData {
    AlphabetIndex notchPosition;                              // Semantic type for [0-25]
    std::array<AlphabetIndex, TRANSFORMER_SIZE> wiring;    // Fixed-size, no heap
};

/**
 * @brief Pure POD struct for reflector configuration.
 */
struct ReflectorData {
    std::array<AlphabetIndex, TRANSFORMER_SIZE> wiring;
};

/**
 * @brief Pure POD struct for a single plugboard pair.
 * Field names align with runtime PlugBoardPair. Uses -1 to indicate unused/unconnected ports.
 */
struct PlugBoardPairData {
    AlphabetIndex sourcePortIndex;      // -1 = unused
    AlphabetIndex destinationPortIndex; // -1 = unused
};

/**
 * @brief Pure POD struct for plugboard configuration.
 * Uses array of structs for better cache locality and semantics.
 */
struct PlugBoardData {
    std::array<PlugBoardPairData, MAX_PLUGBOARD_PAIRS> pairs;
};

/**
 * @brief Pure POD struct for complete machine configuration.
 * All members are primitive types - can be stack-allocated or globally initialized.
 */
struct EnigmaMachineData {
    int rotorCount;
    std::array<AlphabetIndex, MAX_ROTORS> rotorPositions;
    std::array<RotorData, MAX_ROTORS> rotors;
    ReflectorData reflector;
    PlugBoardData plugBoard;
};

} // namespace enigma
```

### Key Design Decisions

1. **Semantic Types with POD Compliance:** All index fields use `AlphabetIndex` (a `using` alias for `int` defined in `EnigmaTypes.hpp`) instead of raw `int`. This adds semantic clarity (indices constrained to `[0-25]` semantics) while maintaining full POD compliance — `std::is_trivially_copyable` still returns true since `AlphabetIndex` is just `int`.

2. **Fixed-Size Arrays:** Use `std::array<T, N>` instead of `std::vector`
   - Reason: `std::array` IS POD when the element type is POD (C++11+). Provides better type safety while maintaining POD compliance.

3. **Array of Structs for PlugBoard:**
   - Use `struct PlugBoardPairData { AlphabetIndex sourcePortIndex; AlphabetIndex destinationPortIndex; }` in an array
   - Field names align with runtime `PlugBoardPair` to enable simple field-by-field copy during conversion
   - Reason: Better cache locality (both values adjacent), clearer semantics, simpler serialization (direct memory copy)

4. **Maximum Capacities:**
   - `MAX_ROTORS = 10`: Historical Enigma used 3, allows future expansion
   - `MAX_PLUGBOARD_PAIRS = 13`: Maximum pairs (26 ports / 2)

5. **Negative One for Unused:**
   - `-1` indicates unused slots
   - Reason: Avoids `std::optional` (non-POD)

## Consequences

### Positive

- **Embedded Support:** Core can initialize on Zephyr RTOS with no heap allocation
- **WASM Compatibility:** Works with Emscripten without `-sALLOW_MEMORY_GROWTH`
- **Serialization Ready:** Direct `memcpy` for save/restore functionality
- **Static Initialization:** Can be initialized at compile time with `{0}`

### Negative

- **Maintenance Overhead:** Two parallel struct hierarchies to keep in sync (though `RotorData` is embedded in `EnigmaMachineData`, reducing duplication).
- **Memory Overhead:** Fixed arrays allocate maximum capacity regardless of actual usage (e.g., 10 rotors allocated even when using 3).
- **API Complexity:** Additional constructor for `EnigmaMachine` accepting `EnigmaMachineData` introduces another code path to maintain and test.
- **Validation Complexity:** POD structs cannot enforce invariants at construction time. Invalid data (e.g., duplicate rotor wiring) must be validated separately before use.

## Implementation Notes

### Implementation Location

POD types will be split across two headers for separation of concerns:

```
include/
├── EnigmaConfig.hpp  // Compile-time constants (MAX_ROTORS, MAX_PLUGBOARD_PAIRS, TRANSFORMER_SIZE)
└── EnigmaData.hpp    // POD DTOs (RotorData, ReflectorData, PlugBoardPairData, EnigmaMachineData)
```

Both headers are part of the `enigma::` namespace. They are included by the public API gateway
(`EnigmaCore.hpp`) so that library consumers automatically have access to both the runtime
configuration and the POD configuration without additional includes.

**Constants moved** from `config/config.hpp` to `include/EnigmaConfig.hpp` (now in `enigma::` namespace):
- `TRANSFORMER_SIZE` (26)
- `MAX_PLUGBOARD_PAIRS` (13) — renamed from `PLUGBOARD_MAX_PAIRS`
- `MAX_ROTORS` (10) — new

**Rationale:**
- `EnigmaConfig.hpp`: Compile-time constants, platform-independent limits
- `EnigmaData.hpp`: POD struct definitions that depend on these constants

### Conversion Utilities

Bidirectional conversion between runtime and POD configuration:

```cpp
// In EnigmaMachineConfig.hpp — Runtime to POD (for serialization/save)
enigma::EnigmaMachineData toData() const;

// In EnigmaMachine.hpp — POD to Runtime (via EnigmaMachine constructor)
explicit EnigmaMachine(const enigma::EnigmaMachineData& data, ILogger* logger = nullptr);
```

The `toData()` method on `EnigmaMachineConfig` converts the runtime configuration to a POD
structure. The reverse direction (POD → Runtime) is handled by the `EnigmaMachine` POD
constructor, which accepts `enigma::EnigmaMachineData` directly and reconstructs the runtime
objects internally. No separate `toConfig()` function is needed.

### Historical Data

Include pre-defined wiring tables for historical Enigma models within the `enigma` namespace:

```cpp
namespace enigma {
namespace historical {

constexpr RotorData ROTOR_I = {
    .notchPosition = 16,
    .wiring = {4, 10, 12, 5, 11, 6, 3, 16, 21, 25, 13, 19, 14, 22, 24, 7, 23, 20, 18, 15, 0, 8, 1, 17, 2, 9}
};
constexpr RotorData ROTOR_II = {/* ... */};
constexpr ReflectorData REFLECTOR_B = {/* ... */};

} // namespace historical
} // namespace enigma
```

### Error Handling

Initial implementation throws exceptions for invalid data. Future iterations will use `std::expected` for exception-free operation.

## Related Decisions

- [ADR 001: Dependency Injection for Assets (IAssetProvider)](001-dependency-injection-for-assets.md) - Complements this decision for filesystem-free initialization
- [ADR 003: Error Handling Strategy (Transition to std::expected)](003-error-handling-strategy.md) - Future error handling for POD constructors

## References

- [C++ Core Guidelines: P.POD](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines.html#p-pod)
- [Plain Old Data (POD) Types Explained in C++](https://www.test-king.com/blog/plain-old-data-pod-types-explained-in-c/)

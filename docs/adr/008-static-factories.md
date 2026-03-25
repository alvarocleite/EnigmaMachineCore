# ADR 008: Static Factory Methods for EnigmaMachine

## Status

Accepted

## Context

Following ADR 007 (Result Type Implementation), we have `enigma::Result<T>` available for error-safe return types. The existing EnigmaMachine constructors throw `std::runtime_error` on failure, which:
- Prevents compilation with `-fno-exceptions` (Task 2.4 requirement)
- Doesn't provide specific error codes for programmatic handling
- Forces consumers to use try/catch for expected failure cases

## Decision

We add static factory methods to EnigmaMachine that return `enigma::Result<EnigmaMachine>`:

```cpp
// Factory method with custom asset provider
static enigma::Result<EnigmaMachine> create(const IAssetProvider& provider,
                                            std::string_view fileName,
                                            std::string_view assetPath = "",
                                            ILogger* logger = nullptr);

// Factory method using internal FileAssetProvider
static enigma::Result<EnigmaMachine> create(std::string_view fileName,
                                              std::string_view assetPath = "",
                                              ILogger* logger = nullptr);
```

### Error Handling

On failure, the factory returns `nonstd::make_unexpected(errorCode)` where `errorCode` is one of:
- `EnigmaError::FileNotFound` - config file or asset not found
- `EnigmaError::ConfigFieldMissing` - missing required fields in config
- `EnigmaError::ConfigCountMismatch` - rotor count mismatch
- `EnigmaError::TransformerSizeMismatch` - component size mismatch
- Other codes from `EnigmaError` enum as appropriate

### Backward Compatibility

The existing throwing constructors remain for backward compatibility with consumers who don't need error-safe initialization. The factory methods delegate to the same internal logic but capture errors as Results instead of throwing.

## Consequences

### Positive
- Enables `-fno-exceptions` compilation (unblocks Task 2.4)
- Provides specific error codes for programmatic error handling
- Consistent with Phase 2 goal of exception-free core
- No breaking changes to existing API

### Negative
- Additional code paths to maintain
- Factory methods duplicate some constructor logic

## Implementation

- **Files modified:**
  - `EnigmaMachine/include/EnigmaMachine.hpp` - Added factory method declarations
  - `EnigmaMachine/src/EnigmaMachine.cpp` - Added factory method implementations
  - `tests/EnigmaMachine/TestEnigmaMachine.cpp` - Added factory tests

- **Tests added:**
  - `FactoryCreateWithProviderSuccess` - verify success with asset provider
  - `FactoryCreateWithPathSuccess` - verify success with file path
  - `FactoryCreateWithProviderFailure` - verify error on missing file
  - `FactoryCreateWithPathFailure` - verify error with path for missing file

## Related ADRs

- ADR 007: Result Type Implementation
- ADR 003: Error Handling Strategy (superseded by 007 and this ADR)

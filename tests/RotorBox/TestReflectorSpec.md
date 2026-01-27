# Reflector Test Specification

## Dependencies
*   **Target Class:** `Reflector`
*   **External Assets:** Requires `assets/Reflector.toml` to be present in the execution directory (handled by CMake copy command).
*   **Base Class:** Inherits behavior from `Transformer`.

## Test Cases

| Test Case Name | Description | Details |
| :--- | :--- | :--- |
| **InitializationAndType** | Verifies correct object creation. | Checks that `getType()` returns `TransformerType::Reflector`. |
| **ForwardTransformation** | Verifies wiring logic against `assets/Reflector.toml`. | Checks specific mappings: `0 ('A') -> 3 ('D')` and `4 ('E') -> 7 ('H')`. |
| **Reciprocity** | Verifies the cryptographic "Self-Inverse" property. | Ensures `transform(transform(x)) == x`. Tested on inputs `0` ('A') and `10` ('K'). |
| **NoRotation** | Verifies the Reflector is a static component. | Calls `rotate()`, asserts return value is `0` (no notch trigger), and verifies `transform(0)` remains constant before and after rotation. |

## Detailed Logic Breakdown

### Test Fixture: `ReflectorTests`
*   **Purpose:** Inherits from `::testing::Test` to provide a shared context for all reflector tests.
*   **Configuration:** Defines `configPath = "assets/Reflector.toml"`, ensuring all tests use the standard configuration copied by the build system.

### Test Case: `InitializationAndType`
*   **Goal:** Verification of object creation.
*   **Check:** Verifies that the created object identifies itself as a `Reflector` (via the `getType()` method), ensuring the base class `Transformer` was initialized correctly.

### Test Case: `ForwardTransformation`
*   **Goal:** Verify that the wiring (Lookup Table) is loaded and functioning.
*   **Verification:** Validates transformation against hardcoded values from `assets/Reflector.toml`:
    *   **Input 0 ('A') -> Output 3 ('D')**
    *   **Input 4 ('E') -> Output 7 ('H')**

### Test Case: `Reciprocity`
*   **Goal:** Verify the fundamental cryptographic property: A Reflector swaps pairs of letters.
*   **The Rule:** If `A` maps to `D`, then `D` *must* map back to `A`.
*   **Significance:** This property allows the Enigma machine to use the *exact same setting* for both encryption and decryption.
*   **Verification:** `f(f(x)) == x`.

### Test Case: `NoRotation`
*   **Goal:** Verify that the Reflector does **not** move.
*   **Context:** Unlike Rotors, the Reflector is stationary.
*   **Checks:**
    1.  `rotate()` returns `0` (never triggers a notch carry).
    2.  **State Invariance:** The transformation result for a given input must remain exactly the same before and after calling `rotate()`. If the result changed, it would imply the internal wiring shifted, which is a bug for a Reflector.

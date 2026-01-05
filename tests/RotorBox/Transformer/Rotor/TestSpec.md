# Rotor Test Specification

## Dependencies
*   **Target Class:** `Rotor`
*   **External Assets:** Requires `assets/Rotor1.toml` to be present in the execution directory (handled by CMake copy command).
*   **Base Class:** Inherits behavior from `Transformer`.

## Test Cases

| Test Case Name | Description | Details |
| :--- | :--- | :--- |
| **InitializationAndType** | Verifies correct object creation. | Checks that `getType()` returns `TransformerType::Rotor`. |
| **BasicTransformation** | Verifies wiring logic against `assets/Rotor1.toml`. | Checks forward mapping `0 -> 3` and reverse mapping `3 -> 0`. |
| **RotationEffect** | Verifies that the component is dynamic. | Calls `rotate()` and asserts that the transformation of a static input (`0`) changes after the rotation. |
| **FullRotationCycle** | Verifies the cyclic nature of the Rotor. | Confirms that 26 calls to `rotate()` return the rotor to its initial state. |
| **SetPosition** | Verifies manual position control. | Ensures `setPosition(5)` produces the same transformation state as starting at `0` and calling `rotate()` 5 times. |

## Detailed Logic Breakdown

### Test Fixture: `RotorTests`
*   **Purpose:** Inherits from `::testing::Test` to provide a shared context for all rotor tests.
*   **Configuration:** Defines `configPath = "assets/Rotor1.toml"`, ensuring all tests use the standard configuration copied by the build system.

### Test Case: `InitializationAndType`
*   **Goal:** Verification of object creation.
*   **Check:** Verifies that the created object identifies itself as a `Rotor` (via the `getType()` method), ensuring the base class `Transformer` was initialized correctly.

### Test Case: `BasicTransformation`
*   **Goal:** Verify that the wiring (Lookup Table) is loaded and functioning for both directions.
*   **Forward Check:** Validates against `assets/Rotor1.toml` where input `0` maps to `3`.
*   **Reverse Check:** Validates the mathematical inverse property where input `3` (which is the output of the forward pass) maps back to `0`.

### Test Case: `RotationEffect`
*   **Goal:** Verify that the Rotor moves and that movement affects the signal path.
*   **Context:** Unlike the Reflector, the Rotor steps after key presses (or when driven).
*   **Verification:**
    1.  Record output of `transform(0)`.
    2.  Call `rotate()`.
    3.  Assert `transform(0)` is NOT equal to the previous result.

### Test Case: `FullRotationCycle`
*   **Goal:** Verify the modulo 26 behavior of the rotor mechanism.
*   **Verification:**
    1.  Record initial state output.
    2.  Loop 26 times, calling `rotate()` in each iteration.
    3.  Assert the final state output matches the initial state output.

### Test Case: `SetPosition`
*   **Goal:** Verify that explicit position setting matches the mechanical stepping logic.
*   **Verification:**
    1.  Test A: `setPosition(5)` -> Measure output.
    2.  Test B: `setPosition(0)` -> `rotate()` x 5 -> Measure output.
    3.  Assert Output A equals Output B.

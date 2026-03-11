# RotorBox Test Specification

## Dependencies
*   **Target Class:** `RotorBox`
*   **Components:** Manages a collection of `Rotor` objects and one `Reflector`.
*   **External Assets:** Requires `assets/Rotor1.toml`, `assets/Rotor2.toml`, `assets/Rotor3.toml`, and `assets/Reflector.toml`.

## Test Cases

| Test Case Name | Description | Details |
| :--- | :--- | :--- |
| **DefaultConstructor** | Verifies default initialization. | Checks if the default constructor loads 3 rotors and 1 reflector. |
| **ParameterizedConstructor** | Verifies custom configuration. | Checks if providing specific files and positions correctly initializes the box. |
| **SingleStepTransformation** | Verifies basic signal flow after one step. | Sets positions to 0,0,0. `keyTransform` steps to 1,0,0 and transforms. |
| **SteppingMechanism** | Verifies the notch-based stepping (odometer). | Sets Rotor 1 to position 25. `keyTransform` should move Rotor 1 to 0 and Rotor 2 to 1. |
| **DoubleStepping** | Verifies carry-over stepping. | Sets Rotor 1 and Rotor 2 to positions that will trigger multiple carries. |
| **RoundTrip** | Verifies the reciprocal nature of Enigma. | Encrypting a character, resetting positions, and encrypting the result should yield the original character. |

## Detailed Logic Breakdown

### Test Fixture: `RotorBoxTests`
*   **Purpose:** Shared context for `RotorBox` tests.
*   **Configuration:** Defines standard rotor files and positions for consistency.

### Test Case: `DefaultConstructor`
*   **Goal:** Ensure the default constructor works (uses Rotor1, 2, 3 and Reflector).
*   **Verification:** Call `keyTransform` and ensure it doesn't crash and returns a value in [0, 25].

### Test Case: `ParameterizedConstructor`
*   **Goal:** Verify custom loading.
*   **Verification:** Load a box with specific positions and verify the transformation matches a manual calculation (or known value).

### Test Case: `SteppingMechanism`
*   **Goal:** Verify that Rotor 2 moves when Rotor 1 hits its notch.
*   **Logic:**
    1.  Set positions to {25, 0, 0}.
    2.  `keyTransform` is called.
    3.  Internally, Rotor 1 rotates to 0. Since 0 is the notch, Rotor 2 rotates to 1.
    4.  Verify that subsequent transformation reflects these new positions.

### Test Case: `RoundTrip`
*   **Goal:** Demonstrate that Enigma is its own inverse if positions are reset.
*   **Logic:**
    1.  Initialize `RotorBox`.
    2.  `c1 = keyTransform(input)`.
    3.  Re-initialize (or reset) `RotorBox` to the SAME starting state.
    4.  `c2 = keyTransform(c1)`.
    5.  Assert `c2 == input`.

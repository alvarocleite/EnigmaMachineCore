# EnigmaMachineConfig Test Specification

## Dependencies
*   **Target Class:** `EnigmaMachineConfig`
*   **Components:** Responsible for parsing TOML files into `RotorConfig`, `ReflectorConfig`, and global settings.
*   **External Assets:** Requires `assets/EnigmaMachineConfig1.toml` and referenced rotor/reflector files.

## Test Cases

| Test Case Name | Description | Details |
| :--- | :--- | :--- |
| **LoadValidConfig** | Verifies successful loading of a valid TOML file. | Loads `assets/EnigmaMachineConfig1.toml` and validates all fields (rotors, positions, plugs). |
| **LoadInvalidConfig** | Verifies error handling for missing/invalid files. | Attempts to load a non-existent file and expects an exception. |
| **RotorConfigProperties** | Verifies parsing logic for individual components. | Checks that loaded rotors have correct wiring size (26) and valid notch positions. |

## Detailed Logic Breakdown

### Test Fixture: `EnigmaMachineConfigTests`
*   **Purpose:** Shared context defining paths to valid and invalid configuration files.
*   **Configs:** 
    *   Valid: `assets/EnigmaMachineConfig1.toml`
    *   Invalid: `assets/non_existent.toml`

### Test Case: `LoadValidConfig`
*   **Goal:** Ensure comprehensive parsing accuracy.
*   **Verification Points:**
    *   `RotorCount` == 3.
    *   `RotorPositions` match `[6, 18, 1]`.
    *   `PlugBoardPairs` are correctly populated (e.g., checks specific pairs like 4<->7).
    *   `Reflector` and `Rotors` vectors are populated.

### Test Case: `LoadInvalidConfig`
*   **Goal:** Verify robustness against bad input.
*   **Logic:** Call `load()` with a bad path. Expect `std::exception` (specifically `toml::file_io_error` or `std::runtime_error`).

### Test Case: `RotorConfigProperties`
*   **Goal:** Deep inspection of the objects created by the config loader.
*   **Logic:** 
    1.  Load valid config.
    2.  Access the first rotor in `getRotors()`.
    3.  Assert `wiring.size() == 26`.
    4.  Assert `notchPosition` is within valid bounds [0, 25].

# Test Specification: EnigmaConfigLoader

## Overview
This specification details the testing requirements for the `EnigmaConfigLoader` class, which is responsible for parsing TOML configuration files and creating `EnigmaMachineConfig` objects. It utilizes the `IAssetProvider` interface to abstract file access.

## Tested Components
*   `EnigmaConfigLoader` (Static Factory)
*   `EnigmaMachineConfig` (DTO Structure)

## Test Scenarios

### 1. Valid Configuration Loading
**Objective:** Verify that a correctly formatted TOML file is parsed into a valid `EnigmaMachineConfig` object.
**Preconditions:** A valid "EnigmaMachineConfig1.toml" exists via the asset provider.
**Steps:**
1.  Call `EnigmaConfigLoader::load()` with the valid file.
2.  **Expected Result:**
    *   Returns a config object.
    *   `rotorCount` matches the file.
    *   `rotorPositions` matches the file.
    *   `rotors` vector size matches count.
    *   Plugboard pairs are correctly populated.

### 2. Invalid File Path
**Objective:** Verify behavior when the file does not exist.
**Steps:**
1.  Call `load()` with a non-existent filename.
2.  **Expected Result:** Throws `std::runtime_error` (propagated from `IAssetProvider`).

### 3. Malformed Rotor Configuration
**Objective:** Verify validation of individual rotor files.
**Steps:**
1.  Mock a rotor file missing the `forward` wiring array.
2.  Call `loadRotor()`.
3.  **Expected Result:** Throws `std::runtime_error` or `toml::exception`.

### 4. Component Type Mismatch
**Objective:** Verify that loading a Reflector file as a Rotor (or vice versa) fails.
**Steps:**
1.  Mock a file with `type="reflector"`.
2.  Call `loadRotor()` on it.
3.  **Expected Result:** Throws `std::runtime_error` with a type mismatch message.

### 5. Invalid Data Types
**Objective:** Verify behavior when fields have wrong types (e.g., string instead of int).
**Steps:**
1.  Mock a config where `RotorCount` is a string "three".
2.  Call `load()`.
3.  **Expected Result:** Throws `toml::type_error` or `std::runtime_error`.

### 6. Logic Validation (Consistency)
**Objective:** Verify logical consistency checks.
**Steps:**
1.  Mock a config where `RotorCount` = 3 but `RotorFiles` list has only 2 items.
2.  Call `load()`.
3.  **Expected Result:** Throws `std::runtime_error` ("Number of rotors ... do not match").

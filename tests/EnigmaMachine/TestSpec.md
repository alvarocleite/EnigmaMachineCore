# EnigmaMachine Test Specification

## Dependencies
*   **Target Class:** `EnigmaMachine`
*   **Components:** Integrates `RotorBox` and `PlugBoard`.
*   **External Assets:** Requires `assets/EnigmaMachineConfig1.toml` and referenced rotor/reflector files.

## Test Cases

| Test Case Name | Description | Details |
| :--- | :--- | :--- |
| **Initialization** | Verifies loading from config file. | Loads `assets/EnigmaMachineConfig1.toml` and checks for no exceptions. |
| **BasicEncryption** | Verifies encryption of a single character. | Encrypts 'A' and checks result against expected value given the initial config. |
| **StringEncryption** | Verifies multi-character encryption. | Encrypts "AAAAA" to observe rotor stepping effect (output should change even if input is constant). |
| **Reciprocity (Decryption)** | Verifies that Encrypt(Encrypt(Msg)) == Msg. | Encrypts "HELLO" -> Cypher. Re-initializes machine. Encrypts Cypher -> Plain. Plain should match "HELLO". |
| **PlugBoardEffect** | Verifies Plugboard influence. | Compares output of a machine with Plugboard vs without Plugboard for same rotor settings. |
| **ProcessBufferSpan** | Verifies block processing. | Compares `processBuffer` result against sequential `keyTransform` calls. |
| **LoggerInjection** | Verifies logger propagation. | Ensures a custom `ILogger` receives stepping events from the engine. |

## Detailed Logic Breakdown
...
### Test Case: `ProcessBufferSpan`
*   **Goal:** Ensure the batch processing API yields identical results to sequential processing.
*   **Procedure:**
    1. Initialize two identical machines.
    2. Process a vector of indexes using `keyTransform` in a loop.
    3. Process the same vector using `processBuffer`.
    4. Assert results are identical.

### Test Case: `LoggerInjectionAndPropagation`
*   **Goal:** Validate the Observer pattern for logging.
*   **Procedure:**
    1. Create a mock `ILogger` that stores logs in a list.
    2. Inject logger into `EnigmaMachine`.
    3. Trigger an action (encryption) that causes a rotor step.
    4. Assert that the mock logger captured a message containing "Rotor" and "stepped".

### Test Fixture: `EnigmaMachineTests`
*   **Purpose:** Setup/Teardown not strictly needed if we initialize new machines per test, but useful for defining common config paths.
*   **Config Path:** `assets/EnigmaMachineConfig1.toml` (standard test asset).

### Test Case: `Initialization`
*   **Goal:** Ensure the complex parsing logic in `parseConfig` works.
*   **Check:** `EnigmaMachine m(configPath);` succeeds.

### Test Case: `BasicEncryption`
*   **Goal:** Single character transformation.
*   **Input:** 0 ('A').
*   **Expectation:** Valid output in range [0, 25]. Specific value depends on config, but must be consistent.

### Test Case: `StringEncryption`
*   **Goal:** Observe dynamic behavior.
*   **Input:** "AAAAA".
*   **Expectation:** Output string should NOT be "BBBBB" (i.e., not a monoalphabetic substitution). The output characters should vary.

### Test Case: `Reciprocity`
*   **Goal:** `Dec(Enc(M)) == M`.
*   **Procedure:**
    1.  `EnigmaMachine m1(configPath);`
    2.  `string ciphertext = "";`
    3.  `ciphertext += m1.keyTransform('H'); ...`
    4.  `EnigmaMachine m2(configPath);` (Reset state)
    5.  `string plaintext = "";`
    6.  `plaintext += m2.keyTransform(ciphertext[0]); ...`
    7.  Assert `plaintext == "HELLO"`.

### Test Case: `PlugBoardEffect`
*   **Goal:** Ensure Plugboard is actually active.
*   **Procedure:**
    1.  Create `m1` with `EnigmaMachineConfig1.toml` (has plugs).
    2.  Create `m2` manually with SAME rotors/reflector but NO plugs.
    3.  Encrypt 'A' with both.
    4.  Assert results are different (assuming 'A' or the result path is affected by the plugs defined in config).
    *   *Note:* Config1 has plugs 4<->7, 18<->20, etc. If we input something that hits these, output differs.

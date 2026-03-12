# Historical Vector Test Specification

## Dependencies
*   **Target Class:** `EnigmaMachine`
*   **Historical Assets:** Requires rotors and reflectors from `assets/historical/`.
*   **Configurations:** Uses specialized TOML files in `assets/historical/`.

## Test Cases

| Test Case Name | Description | Details |
| :--- | :--- | :--- |
| **StandardAAAAA_BDZGO** | Validates the standard Enigma I "AAAAA" vector. | Rotors I-II-III, Reflector B, Positions A-A-A. Expected: `BDZGO`. |
| **ReciprocalBDZGO** | Verifies symmetry using historical output. | Input `BDZGO` with same settings. Expected: `AAAAA`. |
| **LongStringVerification** | Validates stepping over many characters. | 35-character input `THEQUICKBROWNFOX...`. Expected: `OPCILLAZFXLQTDNL...`. |
| **OCAMLVector** | Validates complex setup with plugboard. | Rotors I-II-III, Reflector B, Pos F-U-N, Plugs A-Z. Expected: `OCAML`. |
| **HistoricalReciprocity** | High-level symmetry check. | Encrypt/Decrypt "SECRETENIGMAMESSAGE" with historical settings. |

## Detailed Logic Breakdown

### Test Fixture: `HistoricalVectorTests`
*   **Historical Asset Path:** `assets/historical/`
*   **Helper:** `encryptString` (performs char-to-index conversion and loop).

### Test Case: `StandardAAAAA_BDZGO`
*   **Goal:** Verify machine accuracy against a known historical baseline.
*   **Config:** `EnigmaI_Historical_Config.toml` (R3, R2, R1 order in RotorBox).
*   **Input:** "AAAAA".
*   **Output:** "BDZGO".

### Test Case: `ReciprocalBDZGO`
*   **Goal:** Confirm the machine can decrypt its own historical output.
*   **Procedure:** Use same config as `StandardAAAAA_BDZGO`, but input the resulting ciphertext.
*   **Expectation:** Plaintext matches original "AAAAA".

### Test Case: `LongStringVerification`
*   **Goal:** Ensure cumulative stepping logic (including carries) is correct.
*   **Input:** `THEQUICKBROWNFOXJUMPSOVERTHELAZYDOG` (35 chars).
*   **Expectation:** Matches established `EnigmaCore` baseline `OPCILLAZFXLQTDNLGGLEKDIZOKQKGXIEZKD`.

### Test Case: `OCAMLVector`
*   **Goal:** Verify correct handling of initial offsets and plugboard swaps.
*   **Config:** `EnigmaI_OCAML_Config.toml`.
    *   Rotor Positions (R-L): N(13), U(20), F(5).
    *   Plugboard: A (0) <-> Z (25).
*   **Input:** `YNGXQ`.
*   **Output:** `OCAML`.

### Test Case: `HistoricalReciprocity`
*   **Goal:** General cryptographic consistency.
*   **Check:** `Dec(Enc("SECRET...")) == "SECRET..."` using historical rotor wirings.

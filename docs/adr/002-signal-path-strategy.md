# ADR 002: Signal Path Strategy (Alphabet Index vs. Raw Chars)

## Status
Accepted

## Context
The internal signal path of the Enigma Machine involves passing a character through multiple rotors and a reflector. There are two primary ways to represent this signal:
1.  **Raw Characters:** Passing `char` values (e.g., 'A', 'B') and performing character arithmetic.
2.  **Alphabet Indices:** Passing `AlphabetIndex` values representing the index in the alphabet (e.g., 0 to `TRANSFORMER_SIZE - 1`).

## Decision
We will use **Alphabet Indices** (`AlphabetIndex` values, 0 to `TRANSFORMER_SIZE - 1`) for all internal signal transformations within the `Transformer`, `RotorBox`, and `PlugBoard` classes.

1.  **Semantic Consistency:** Transformations are mathematical mappings within a set. Using indices makes this mapping explicit.
2.  **Performance:** Avoids repeated conversions between ASCII/UTF-8 and numeric indices during the hot path of encryption.
3.  **Flexibility:** Easily supports non-standard alphabet sizes (e.g., 29 characters) by simply changing the `TRANSFORMER_SIZE` constant and updating the index range.
4.  **Boundary Responsibility:** The `EnigmaMachine::keyTransform` (or a higher-level UI wrapper) is responsible for converting input `char` to `int` and back to `char` for the final output.

## Consequences
*   **Validation:** All internal components must enforce that inputs are within the valid [0, TRANSFORMER_SIZE-1] range.
*   **Type Safety:** While `int` is used currently, future refactoring should consider strong typing (e.g., an `AlphabetIndex` class) to prevent accidental mixing of indices and raw values.

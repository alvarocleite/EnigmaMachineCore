# PlugBoard Test Specification

## Dependencies
*   **Target Class:** `PlugBoard`
*   **External Assets:** None required for unit testing (configuration is injected via constructor).

## Test Cases

| Test Case Name | Description | Details |
| :--- | :--- | :--- |
| **DefaultInitialization** | Verifies default state (Identity). | Checks that an empty PlugBoard maps every character to itself. |
| **CustomConfiguration** | Verifies logic with explicit pairs. | Configures pairs like (A, Z), (B, Y) and verifies `swap('A') == 'Z'` etc. |
| **Reciprocity** | Verifies the "Self-Inverse" property. | Ensures `swap(swap(x)) == x` for both plugged and unplugged characters. |
| **ConflictHandling** | Verifies robustness against invalid configs. | Attempts to connect one port to multiple destinations (e.g., A-B, A-C). Should throw `std::invalid_argument`. |
| **SelfLoop** | Verifies robustness against self-connections. | Attempts to connect a port to itself (e.g., A-A). Should be ignored or treated as identity. |
| **OutOfBounds** | Verifies handling of invalid inputs. | Inputs < 0 or >= 26 should be returned unchanged. |

## Detailed Logic Breakdown

### Test Fixture: `PlugBoardTests`
*   **Purpose:** Provides a clean environment for each test.
*   **Helpers:** May provide helper functions to generate `PlugBoardPair` arrays easily.

### Test Case: `DefaultInitialization`
*   **Goal:** Ensure a default `PlugBoard` acts as a pass-through.
*   **Check:** Iterate 0..25, assert `swap(i) == i`.

### Test Case: `CustomConfiguration`
*   **Goal:** Verify wiring logic.
*   **Setup:** Create pairs `{{0, 25}, {1, 2}}` (A-Z, B-C).
*   **Check:**
    *   `swap(0)` returns 25.
    *   `swap(25)` returns 0.
    *   `swap(1)` returns 2.
    *   `swap(2)` returns 1.
    *   `swap(3)` returns 3 (unaffected).

### Test Case: `Reciprocity`
*   **Goal:** `f(f(x)) == x`.
*   **Check:** For all valid inputs, applying `swap` twice restores original value.

### Test Case: `ConflictHandling`
*   **Goal:** Ensure physical impossibility is handled (one plug cannot go to two places).
*   **Setup:** Pairs `{{0, 1}, {0, 2}}` (A-B, A-C).
*   **Expectation:** Construction throws `std::invalid_argument`.
*   **Check:** Verify exception is thrown.

### Test Case: `SelfLoop`
*   **Goal:** Ensure plugging a letter to itself is safe (or ignored).
*   **Setup:** Pair `{{0, 0}}`.
*   **Check:** `swap(0)` returns 0.

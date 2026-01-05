# Transformer Test Specification

## Dependencies
*   **Target Class:** `Transformer` (Abstract Base Class)
*   **Derived Classes:** `Rotor`, `Reflector`
*   **External Assets:** None (Base class logic is independent of specific assets).

## Test Cases

| Test Case Name | Description | Details |
| :--- | :--- | :--- |
| **DefaultInitialization** | Verifies the initial state of the base class. | Checks that `getType()` returns `TransformerType::NotDefined`. |
| **LUTSize** | Verifies the capacity of the transformation lookup tables. | Checks that `sizeOfTransformLUT()` returns `52` (2 * 26). |
| **VirtualDestructor** | Ensures safe deletion through base pointer. | (Implicit) Verifies that the destructor is virtual to prevent memory leaks in derived classes. |

## Detailed Logic Breakdown

### Test Fixture: `TransformerTests`
*   **Purpose:** Provides a testing environment for the `Transformer` base class. Since `Transformer` is abstract, these tests typically use a minimal concrete implementation (`ConcreteTransformer`) to verify base class logic.

### Test Case: `DefaultInitialization`
*   **Goal:** Verify that the constructor correctly initializes the `type` member.
*   **Check:** Asserts that a fresh instance of a `Transformer` subclass correctly identifies its base type as `NotDefined` before specialization.

### Test Case: `LUTSize`
*   **Goal:** Verify the dimensions of the internal transformation array.
*   **Context:** The Enigma machine uses a 26-character alphabet. The LUT stores both forward and reverse transformations.
*   **Verification:** Asserts that `sizeOfTransformLUT()` returns exactly `52`, confirming that the `transformLUT[2][26]` array is correctly sized in memory.

### Test Case: `VirtualDestructor`
*   **Goal:** Prevent undefined behavior when deleting objects polymorphically.
*   **Verification:** While difficult to test directly with GTest without specialized tools, the specification mandates that the base class provides a virtual destructor to support derived classes like `Rotor` and `Reflector`.

# Test Specification: EnigmaObserver

## Overview
This specification details the testing requirements for the `IEnigmaObserver` interface and its integration with the `EnigmaMachine`. The observer pattern is used to notify external components (like UIs or loggers) about internal state changes.

## Tested Components
*   `EnigmaMachine` (Subject)
*   `IEnigmaObserver` (Interface)

## Test Scenarios

### 1. Registration and Event Notification
**Objective:** Verify that a registered observer receives notifications for both rotor stepping and character encryption.
**Steps:**
1.  Initialize an `EnigmaMachine`.
2.  Register a `MockObserver`.
3.  Perform a character transformation using `keyTransform()`.
4.  **Expected Result:**
    *   `onCharEncrypted` is called once with the correct input/output characters.
    *   `onRotorStepped` is called at least once (for the rightmost rotor).

### 2. Unregistration Behavior
**Objective:** Verify that an observer stops receiving notifications after being removed.
**Steps:**
1.  Register a `MockObserver`.
2.  Remove the `MockObserver` using `removeObserver()`.
3.  Perform a character transformation.
4.  **Expected Result:**
    *   The observer's event counters remain at zero.

### 3. Multiple Observers (Implicit)
**Objective:** (Implementation detail) Ensure the internal collection of observers correctly handles broadcast.
*Note: Covered by the fact that EnigmaMachine itself acts as an observer to its internal RotorBox and propagates those events.*

# Architecture & Design

This document outlines the architectural design and key decisions behind the **EnigmaMachineCore** project. It provides an overview of the system's structure, components, and interactions, as well as the rationale behind major design choices.

Significant architectural decisions, including their context, rationale, and consequences, are tracked in our [ADRs (Architectural Decision Records)](adr/ADRs.md).

## 1. System Overview

The **EnigmaMachineCore** is a C++20 implementation of the Enigma cipher. It separates the cryptographic "Business Logic" from the application layer and the physical storage layer.

### 1.1. Component Architecture

The system is divided into a reusable library (`EnigmaCore`) and a thin CLI Wrapper (`EnigmaMachineCore`).

![Component Diagram](diagrams/output/EnigmaMachineCore_Component_Diagram.svg)

### 1.2. Deployment Architecture

The system is designed for flexibility across different platforms. The deployment strategy varies between standard OS environments and embedded targets.

![Deployment View](diagrams/output/EnigmaMachineCore_Deployment_View.svg)

### 1.3. API Encapsulation (Minimal Surface Area)

The library follows a "Strict Facade" approach. To ensure binary stability and security, only the high-level control interfaces are exported.

*   **Public API:** `EnigmaMachine` (Execution), `IEnigmaObserver` (Monitoring), and `IAssetProvider` (Extensibility). See [Library API Specification](Library_API.md) for details.
*   **Hidden Internals:** All internal components (`Rotor`, `PlugBoard`, `RotorBox`, `EnigmaConfigLoader`) and raw configuration structs (`EnigmaMachineConfig`) are hidden from the binary's export table.
*   **Symbol Visibility:** The library is compiled with `hidden` visibility by default. Only classes annotated with `ENIGMACORE_EXPORT` are visible to consumers.

## 2. Use Cases

The system serves two primary actors:
*   **Operator:** Encrypts and decrypts messages.
*   **Technician:** Configures the machine's state (Rotors, Plugs).

![Use Case Diagram](diagrams/output/EnigmaCore_Use_Case_Diagram.svg)

## 3. Architectural Design

The architecture is driven by **Separation of Concerns** and **Testability**, avoiding tight coupling between data, logic, and the filesystem.

![Class Diagram](diagrams/output/EnigmaMachineCore_Class_Diagram.svg)

### 3.1. Data vs. Logic (SRP)

The system strictly separates configuration data from the logic required to load it.

*   **EnigmaMachineConfig (DTO):** A simple `struct` that holds the machine's state (Rotor positions, wiring, plug pairs).
    *   *Why:* Keeps the data structure pure and free from dependencies on file parsers (like TOML).
*   **EnigmaConfigLoader (Factory):** A dedicated service responsible for parsing files and creating configuration objects.
    *   *Why:* Centralizes validation logic and allows the loading strategy to change (e.g., adding JSON support) without modifying the data structure or the core engine.

### 3.2. IO Abstraction (Dependency Injection)

The core engine does not access the filesystem directly.

*   **IAssetProvider:** An interface that defines how to retrieve configuration content.
*   **FileAssetProvider:** The standard implementation that reads from disk.
    *   *Why:* This allows the engine to run in environments without a standard filesystem (e.g., Embedded, WebAssembly) and enables unit testing with in-memory mock data (`MockAssetProvider`).

### 3.3. Core Domain (Facade)

*   **EnigmaMachine:** Acts as a **Facade**, providing a simple API (`keyTransform`) while hiding the complexity of the internal components.
*   **RotorBox:** Manages the mechanical rules (rotor sequence, stepping logic) and orchestrates the signal path.
*   **Transformer:** Uses the **Strategy Pattern** to treat Rotors and Reflectors uniformly. It provides dedicated `transformForward` and `transformReverse` paths to optimize CPU branch prediction. Additionally, expensive modulo operations in the signal path are replaced with conditional subtractions to maximize throughput. Defined in `RotorBox/include/Transformer.hpp`.

### 3.4. Event Handling (Observer Pattern)

To allow external systems (User Interfaces, Loggers) to react to internal state changes without coupling the core engine to them, the system implements the **Observer Pattern**.

*   **IEnigmaObserver:** An abstract interface defining callbacks for key events (`onRotorStepped`, `onCharEncrypted`).
*   **Proxy Pattern:** To minimize redundant logic, `EnigmaMachine` implements `IEnigmaObserver` and acts as a proxy. It registers itself as the sole observer of `RotorBox`. When `RotorBox` triggers a rotor step event, `EnigmaMachine` receives it and forwards it to all externally registered observers.
*   **Decoupling:** The `EnigmaMachine` maintains a list of external observers but knows nothing about their concrete implementation. This allows the CLI to print logs or a GUI to animate spinning rotors simply by implementing this interface.

### 3.5. Configuration Loading

The initialization process orchestrates the `EnigmaConfigLoader`, `IAssetProvider`, and the underlying TOML parser to construct a valid machine state.

![Configuration Loading Sequence](diagrams/output/Enigma_Config_Loading_Sequence.svg)

### 3.6. Error Handling Strategy

The system uses **Exceptions** rather than error codes to handle runtime failures.

*   **std::runtime_error:** Thrown for recoverable errors such as missing configuration files, invalid TOML formats, or logical errors (e.g., misconfigured rotor wiring).
*   **RAII (Resource Acquisition Is Initialization):** Constructors are responsible for establishing a valid state. If initialization fails (e.g., invalid arguments), the object is not created, preventing the system from entering an inconsistent state.

## 4. Execution Flow

The encryption logic follows the physical signal path of the historical machine:

1.  **Plugboard Entry:** Input character is swapped.
2.  **Rotor Stepping:** Mechanical movement happens *before* the electrical signal passes.
3.  **Forward Pass:** Signal passes through Rotors (Right -> Left).
4.  **Reflection:** Signal hits the Reflector.
5.  **Reverse Pass:** Signal returns through Rotors (Left -> Right).
6.  **Plugboard Exit:** Output character is swapped again.

### 4.1. Rotor Stepping Logic

The mechanical stepping mechanism is the most complex state transition in the system and faithfully reproduces the behavior of the historical Enigma machine, including the **double-stepping phenomenon**.

On each key press, the stepping logic operates as follows:

1.  **Pre-step notch evaluation**
    Before any rotor is rotated, each rotor’s current position is checked to determine whether it is at its notch. These notch states are captured **prior to rotation** and drive all subsequent stepping decisions.

2.  **Rightmost rotor rotation**
    The rightmost (fast) rotor always rotates on every key press, regardless of notch positions.

3.  **Carry-over rotation**
    A rotor rotates if the rotor immediately to its right was at its notch position before the step. This models the mechanical carry behavior of the Enigma.

4.  **Double stepping**
    If a rotor is at its own notch position and is not the leftmost rotor, it also rotates.

5.  **Post-step notification**
    After all rotations are applied, observers are notified with the updated position of each rotor.

This approach ensures historically accurate rotor motion by:
- Evaluating notch positions **before** rotation
- Always stepping the rightmost rotor
- Correctly modeling carry-over and double-stepping behavior

As a result, the rotor movement and resulting cipher output match authentic Enigma machine behavior.

![State Machine](diagrams/output/Enigma_Rotor_State_Machine.svg)

### 4.2. Signal Path Sequence

![Encryption Sequence](diagrams/output/Enigma_Encryption_Sequence_Diagram.svg)

For a detailed visual and technical breakdown of the electrical path and component roles, see [Technical Workings of the Enigma Machine](EnigmaMachineWorkings.md).

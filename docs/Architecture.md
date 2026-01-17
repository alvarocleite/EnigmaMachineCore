# Architecture & Design

## 1. System Overview

The **EnigmaMachineCore** is designed as a modular, configurable cryptographic engine. It separates the "business logic" of the Enigma cipher (Rotors, Reflectors, Plugboard) from the application layer (CLI).

## 2. Use Cases

The primary actors are the **Operator** (who sends messages) and the **Technician** (who configures the machine).

![Use Case Diagram](diagrams/output/EnigmaCore_Use_Case_Diagram.svg)

## 3. Component Architecture

The system is built using an Object-Oriented approach with C++20.
*   **EnigmaMachine:** The facade.
*   **RotorBox:** Manages the complexity of the signal path through multiple rotors and the reflector.
*   **PlugBoard:** Handles the initial and final character swaps.
*   **Transformer:** The abstract base class for any component that transforms a signal (Rotor, Reflector).

![Class Diagram](diagrams/output/EnigmaMachineCore_Class_Diagram.svg)

## 4. Execution Flow

The encryption process follows a strict physical signal path simulation:
1.  **Plugboard Entry:** Input character is swapped.
2.  **Rotor Stepping:** Mechanical movement occurs *before* the electrical signal passes.
3.  **Forward Pass:** Signal goes through Rotors right-to-left.
4.  **Reflection:** Signal hits the Reflector.
5.  **Reverse Pass:** Signal returns through Rotors left-to-right.
6.  **Plugboard Exit:** Output character is swapped again.

![Encryption Sequence](diagrams/output/Enigma_Encryption_Sequence_Diagram.svg)

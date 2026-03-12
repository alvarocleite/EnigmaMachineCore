# Technical Workings of the Enigma Machine

This document explains the internal mechanics and signal flow of the Enigma machine simulation, supported by technical diagrams.

## 1. Complete Signal Flow

The Enigma machine operates on a reciprocal electrical signal path. This means that with the same configuration (rotor selection, positions, ring settings, and plugboard pairs), the process of encryption and decryption is identical.

### Signal Path Journey
1.  **Keyboard Entry:** The operator presses a key.
2.  **Plugboard (Entry):** The signal passes through the plugboard, where it may be swapped with another letter if a cable is connected.
3.  **Entry Wheel (ETW):** Maps the keyboard/plugboard signal to the input of the rotor assembly.
4.  **Rotor Stepping:** Before the signal passes through the rotors, the mechanical stepping mechanism advances the rightmost rotor (and potentially others).
5.  **Rotor Assembly (Forward):** The signal passes through each rotor from right to left.
6.  **Reflector (UKW):** The signal is reflected back, essentially swapping the character and sending it on a return path.
7.  **Rotor Assembly (Reverse):** The signal passes back through the rotors from left to right using their inverse mappings.
8.  **Plugboard (Exit):** The signal passes through the plugboard a second time (using the same cable connections).
9.  **Lampboard:** The resulting signal illuminates a lamp on the panel.

![Signal Flow Diagram](diagrams/signal-flow.svg)

---

## 2. RotorBox Organization

The `RotorBox` manages the assembly of rotors and the reflector. In our simulation, Rotor 0 represents the rightmost (fast) rotor, which increments with every key press.

### Components
*   **Rotors:** Each rotor has internal wiring that performs a substitution cipher. As rotors rotate, the effective substitution mapping shifts.
*   **Notches:** Physical notches on the rotors trigger the turnover of the adjacent rotor to the left.
*   **The Reflector:** A static component at the end of the chain that ensures the signal returns through the rotors, providing the machine's reciprocal property.

![RotorBox Organization](diagrams/rotorbox-organization.svg)

---

## 3. Plugboard (Steckerbrett) Structure

The plugboard provides an additional layer of security by allowing the operator to swap pairs of letters before they enter the rotor assembly and after they exit.

*   **Pairs:** If 'A' is plugged to 'Z', then 'A' becomes 'Z' and 'Z' becomes 'A'.
*   **Identity:** If a letter has no cable connected, it passes through unchanged (Identity mapping).
*   **Constraints:** Each socket can only accommodate one cable end (no letter can be part of two pairs).

![Plugboard Structure](diagrams/plugboard-structure.svg)

---

## 4. References and Further Reading

The following resources provide in-depth technical details on the mechanical and electrical operation of the historical Enigma machine:

*   **Crypto Museum:** [Enigma Signal Path](https://www.cryptomuseum.com/crypto/enigma/index.htm) - Detailed technical analysis of the machine's wiring and components.
*   **Tony Sale's Codes and Ciphers:** [The Enigma Military Machine](http://www.codesandciphers.org.uk/enigma/index.htm) - Original technical specifications and diagrams.
*   **Cipher Machines and Cryptology:** [Enigma Procedure](https://www.ciphermachinesandcryptology.com/en/enigmaproc.htm) - Explanation of the stepping mechanism and double-stepping anomaly.
*   **Wikipedia:** [Enigma Machine](https://en.wikipedia.org/wiki/Enigma_machine) - Comprehensive overview of the machine's history and variations.

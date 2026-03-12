# Historical Enigma Data

This document provides the sources and details for the historical Enigma machine components (rotors and reflectors) included in the `assets/historical/` directory.

## Machine: Enigma I (Wehrmacht/Luftwaffe)

The Enigma I was the standard machine used by the German Army (Wehrmacht) and Air Force (Luftwaffe) during World War II. It featured three rotors selected from a set of five and one of several reflectors.

### Rotor Wiring & Notches

| Rotor | Wiring (A-Z) | Notch (Pos) | Notch (Char) |
| :--- | :--- | :---: | :---: |
| **I** | `EKMFLGDQVZNTOWYHXUSPAIBRCJ` | 16 | Q |
| **II** | `AJDKSIRUXBLHWTMCQGZNPYFVOE` | 4 | E |
| **III** | `BDFHJLCPRTXVZNYEIWGAKMUSQO` | 21 | V |
| **IV** | `ESOVPZJAYQUIRHXLNFTGKDCMWB` | 9 | J |
| **V** | `VZBRGITYUPSDNHLXAWMJQOFECK` | 25 | Z |

*Note: The `notchPosition` in the TOML files represents the zero-based index where the notch is located on the rotor's physical ring.*

### Reflector Wiring (Umkehrwalze)

| Reflector | Wiring (A-Z) | Description |
| :--- | :--- | :--- |
| **UKW-A** | `EJMZALYXVBWFCRQUONTSPIKHGD` | Early pre-war reflector. |
| **UKW-B** | `YRUHQSLDPXNGOKMIEBFZCWVJAT` | Standard wartime reflector. |
| **UKW-C** | `FVPJIAOYEDRZXWGCTKUQSBNMHL` | Late-war reflector. |

## Data Format

In the `assets/historical/*.toml` files, the wiring is converted from the A-Z strings to zero-based integer arrays (`0` for A, `25` for Z).

Example conversion for Rotor I:
`E` -> `4`, `K` -> `10`, `M` -> `12`, ...

## Sources

The wiring and notch data were gathered from the following authoritative sources:

1.  **Crypto Museum**: [Enigma wiring](https://www.cryptomuseum.com/crypto/enigma/wiring.htm)
2.  **Cipher Machines and Cryptology**: [Enigma Rotors and Wiring](https://www.ciphermachinesandcryptology.com/en/enigmaproc.htm)
3.  **Tony Sale's Codes and Ciphers**: [Technical Details of the Enigma Machine](http://www.codesandciphers.org.uk/enigma/rotorspec.htm)
4.  **Wikipedia**: [Enigma rotor details](https://en.wikipedia.org/wiki/Enigma_rotor_details)

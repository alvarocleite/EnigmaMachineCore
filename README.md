# EnigmaMachineCore

EnigmaMachineCore is a C++ project that simulates the behavior of the Enigma Machine. 

Enigma Machine was a cipher devise used for secure communication in the first hof of the 20th century. For more details about [Enigma Machine](https://en.wikipedia.org/wiki/Enigma_machine).

This provides a modular and extensive implementation of Enigma's core components and they behaviors.

**Features:**
- **Faithful Simulation:** Accurately simulates the rotor stepping, wiring, and reflector logic of the Enigma machine.
- **Configurable Transformers:** Load rotor and reflector configurations from TOML files for easy customization.
- **Modular Design:** Clean separation of components (rotors, reflectors, RotorBox logic) for extensibility and testing.
- **C++17 and CMake:** Modern C++ codebase with cross-platform build support.

## Requirements  

- C++17 compatible compiler
- [CMake](https://cmake.org/)
- git
- [toml11](https://github.com/ToruNiina/toml11) (included as a submodule)

## Usage

1. Clone the project
```bash
git clone --recurse-submodules https://github.com/alvarocleite/EnigmaMachineCore.git && cd EnigmaMachineCore
```

2. Build the project:
```bash
mkdir build
cd build
cmake ..
make
```

3. Run the project
```bash
./Enigma-Machine-Core
```

## Customization

Rotors and Reflectors could be created and customized. Use the `assets` directory to put those customized components.

## License

[Apache-2.0](LICENSE)

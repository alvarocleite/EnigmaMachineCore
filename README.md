# EnigmaMachineCore

EnigmaMachineCore is a C++ project that simulates the behavior of the Enigma Machine. 

The Enigma Machine was a cipher device used for secure communication in the first half of the 20th century. For more details, see [Enigma Machine](https://en.wikipedia.org/wiki/Enigma_machine).

This project provides a modular and extensive implementation of Enigma's core components and their behaviors.

**Features:**
- **Faithful Simulation:** Accurately simulates the rotor stepping, wiring, and reflector logic of the Enigma machine.
- **Configurable Transformers:** Load rotor and reflector configurations from TOML files for easy customization.
- **Modular Design:** Clean separation of components (rotors, reflectors, RotorBox logic) for extensibility and testing.
- **C++20 and CMake:** Modern C++ codebase with cross-platform build support.

## Getting Started

### Prerequisites
To build and run this project, you will need a **C++20 compatible compiler**, **CMake 3.15+**, and **Git**. 

For a complete list of dependencies and detailed environment setup, please refer to the [Building and Testing guide](docs/Building.md).

### Quick Start
```bash
# 1. Clone the project
git clone --recurse-submodules https://github.com/alvarocleite/EnigmaMachineCore.git
cd EnigmaMachineCore

# 2. Configure and Build (Release)
cmake -DCMAKE_BUILD_TYPE=Release -S . -B build/release
cmake --build build/release

# 3. Run Tests (Optional, requires Debug build)
# cmake -DCMAKE_BUILD_TYPE=Debug -S . -B build/debug
# cmake --build build/debug
# cd build/debug && ctest
```

For more detailed build configurations, VS Code integration, and an overview of the build architecture, see [docs/Building.md](docs/Building.md). For details on the testing infrastructure, see the [Testing Guide](docs/Testing.md).

## Documentation

If you have Doxygen installed, you can generate the project documentation.

**Using the modern CMake CLI:**
```bash
cmake --build build/debug --target doxygen
```

**Using the traditional Makefile approach:**
```bash
cd build/debug
make doxygen     # Generate only the documentation
```

HTML documentation is generated in: `docs/doxygen-gen-files/html/index.html`

## Customization

Rotors and Reflectors can be created and customized. Use the `assets` directory to place your customized components.

***Note:** The build system automatically copies the `assets/` folder to the binary directory after each build, ensuring the executable always has access to the latest configuration files.*

## License

[Apache-2.0](LICENSE)

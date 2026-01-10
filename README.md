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

# 2. Configure and Build
# Choose -DCMAKE_BUILD_TYPE=Release for production or Debug for debugging and for testing
cmake -DCMAKE_BUILD_TYPE=Release -S . -B build
cmake --build build

# 3. Run Tests
cd build && ctest --output-on-failure

# 4. Run the Application
./EnigmaMachineCore --help
./EnigmaMachineCore -m "HELLO" --debug
```

For more detailed build configurations and VS Code integration, see [docs/Building.md](docs/Building.md).

## Running Tests

The project uses **Google Test (GTest)** for unit testing and **CTest** for execution. To run the tests, navigate to the build directory and use the following command:

```bash
cd build
ctest --output-on-failure
```

For a detailed overview of the testing infrastructure, VS Code integration, and how to add new tests, refer to the [Testing Guide](docs/Testing.md).

## Documentation

If you have Doxygen installed, you can generate the project documentation.

**Using the modern CMake CLI:**
```bash
cmake --build build --target doxygen
```

**Using the traditional Makefile approach:**
```bash
cd build
make doxygen     # Generate only the documentation
```

HTML documentation is generated in: `docs/doxygen-gen-files/html/index.html`

## Customization

Rotors and Reflectors can be created and customized. Use the `assets` directory to place your customized components.

Alternatively, you can specify a custom assets directory at runtime using the `--assets` command-line option.

***Note:** The build system automatically copies the `assets/` folder to the binary directory after each build, ensuring the executable always has access to the latest configuration files.*

## License

[Apache-2.0](LICENSE)

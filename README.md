# EnigmaMachineCore

EnigmaMachineCore is a C++ project that simulates the behavior of the Enigma Machine. 

The Enigma Machine was a cipher device used for secure communication in the first half of the 20th century. For more details, see [Enigma Machine](https://en.wikipedia.org/wiki/Enigma_machine).

This project provides a modular and extensive implementation of Enigma's core components and their behaviors.

**Features:**
- **Faithful Simulation:** Accurately simulates the rotor stepping, wiring, and reflector logic of the Enigma machine.
- **Configurable Transformers:** Load rotor and reflector configurations from TOML files for easy customization.
- **Modular Design:** Clean separation of components (rotors, reflectors, RotorBox logic) for extensibility and testing.
- **C++20 and CMake:** Modern C++ codebase with cross-platform build support.

## Prerequisites & Dependencies

To build and run this project, you will need the following tools and libraries:

### Core Build Tools
- **C++ Compiler:** Requires a C++20 compatible compiler (GCC 10+, Clang 10+, or MSVC 2019+).
- **[CMake](https://cmake.org/):** Version 3.15 or higher is required to generate the build files.
- **[GDB](https://www.sourceware.org/gdb/):** Essential for debugging logic on Linux environments.
- **Git:** Required for version control and to manage the project's submodules.

### Libraries
- **[toml11](https://github.com/ToruNiina/toml11):** A powerful C++11 header-only library for TOML. ***Note:** This is included as a git submodule in the `external/` directory.*

### Documentation & Visualization (Optional)
- **[Doxygen](https://www.doxygen.nl/):** Used for generating the HTML/XML API documentation.
- **[Graphviz](https://graphviz.org/):** Enables Doxygen to generate interactive call graphs and class diagrams.
- **[PlantUML](https://plantuml.com/):** Required if you wish to process and view manual diagrams within the `/docs` folder.

## Usage

### 1. Clone the project
```bash
git clone --recurse-submodules https://github.com/alvarocleite/EnigmaMachineCore.git
cd EnigmaMachineCore
```

### 2. Build the project
The project supports different build types. It is recommended to use separate directories for Debug and Release builds.

**Release Build (Optimized for performance):**
```bash
cmake -DCMAKE_BUILD_TYPE=Release -S . -B build/release
cmake --build build/release
```

**Debug Build (With debug symbols for development):**
```bash
cmake -DCMAKE_BUILD_TYPE=Debug -S . -B build/debug
cmake --build build/debug
```

### 3. Run the project
```bash
# From the root directory:
./build/release/EnigmaMachineCore
```

## VS Code Integration

The project includes pre-configured settings for Visual Studio Code:

- **Build:** Press `Ctrl+Shift+B` to trigger the default build (Debug).
- **Debug:** Press `F5` to start debugging with GDB. This will automatically build the project in Debug mode before launching.
- **Tasks:** Use `Run Task` to access specific commands like `Build (Release)` or `CMake: Configure`.

## Documentation

Upon building with documentation enabled, HTML documentation is generated in:
`docs/doxygen-gen-files/html/index.html`

**Using the modern CMake CLI:**
```bash
cmake --build build/debug --target doxygen
```

**Using the traditional Makefile approach:**
```bash
make doxygen     # Generate only the documentation
make full_build  # Build everything (executable + documentation)
```

## Customization

Rotors and Reflectors can be created and customized. Use the `assets` directory to place your customized components.

***Note:** The build system automatically copies the `assets/` folder to the binary directory after each build, ensuring the executable always has access to the latest configuration files.*

## License

[Apache-2.0](LICENSE)

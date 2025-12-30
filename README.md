# EnigmaMachineCore

EnigmaMachineCore is a C++ project that simulates the behavior of the Enigma Machine. 

Enigma Machine was a cipher devise used for secure communication in the first hof of the 20th century. For more details about [Enigma Machine](https://en.wikipedia.org/wiki/Enigma_machine).

This provides a modular and extensive implementation of Enigma's core components and they behaviors.

**Features:**
- **Faithful Simulation:** Accurately simulates the rotor stepping, wiring, and reflector logic of the Enigma machine.
- **Configurable Transformers:** Load rotor and reflector configurations from TOML files for easy customization.
- **Modular Design:** Clean separation of components (rotors, reflectors, RotorBox logic) for extensibility and testing.
- **C++20 and CMake:** Modern C++ codebase with cross-platform build support.

## Requirements  

- C++20 compatible compiler
- [CMake](https://cmake.org/) (installed in the system)
- [Doxygen](https://www.doxygen.nl/index.html) (installed in the system)
- [Graphviz](https://graphviz.org/) (optional, for documentation diagrams)
- [PlantUML](https://plantuml.com/) (optional, for manual diagrams)
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

# Choose a build target:
make             # Build only the executable (Enigma-Machine-Core)
make doxygen     # Generate only the documentation
make full_build  # Build everything (executable + documentation)
```

3. Run the project
```bash
./Enigma-Machine-Core
```

## Documentation

Upon running `make doxygen` or `make full_build`, HTML documentation is generated in:
`docs/doxygen-gen-files/html/index.html`

The configuration (`Doxyfile.in`) is a CMake template that ensures consistency:
- **Automatic Versioning:** The project version is synced directly from `CMakeLists.txt`.
- **Portability:** Tool paths (like PlantUML) are detected automatically by CMake for the current machine.
- **Output:** The final `Doxyfile` is generated in the build directory, keeping the source tree clean.

The generated documentation provides a comprehensive developer guide:
- **Internal Logic:** Private members and static functions are documented to explain internal behavior.
- **Visualizations:** Interactive call graphs and inheritance trees are generated (requires Graphviz).
- **Quality Control:** The build process will issue warnings for undocumented code to ensure high standards.

## Customization

Rotors and Reflectors could be created and customized. Use the `assets` directory to put those customized components.

## License

[Apache-2.0](LICENSE)

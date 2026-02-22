# EnigmaMachineCore

EnigmaMachineCore is a C++ project that simulates the behavior of the Enigma Machine. 

The Enigma Machine was a cipher device used for secure communication in the first half of the 20th century. For more details, see [Enigma Machine](https://en.wikipedia.org/wiki/Enigma_machine).

This project provides a modular and extensive implementation of Enigma's core components and their behaviors.

**Features:**
- **Faithful Simulation:** Accurately simulates the rotor stepping, wiring, and reflector logic of the Enigma machine.
- **Configurable Transformers:** Load rotor and reflector configurations from TOML files for easy customization.
- **Modular Design:** Clean separation of components (rotors, reflectors, RotorBox logic) for extensibility and testing.
- **Advanced Architecture:** Uses Dependency Injection and DTOs to separate data, logic, and IO access.
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
# Standard build only includes the application and core library
cmake -DCMAKE_BUILD_TYPE=Release -S . -B build
cmake --build build

# 3. Run the Application
./build/EnigmaMachineCore --help
./build/EnigmaMachineCore -m "HELLO" --debug
```

For more detailed build configurations and VS Code integration, see [docs/Building.md](docs/Building.md).

## Library Usage

To use `EnigmaCore` as a library in your own project, include the namespaced public API gateway:

```cpp
#include <EnigmaMachineCore/EnigmaCore.hpp>

int main() {
    // 1. Initialize a standard machine (auto-resolves assets if installed)
    EnigmaMachine machine;

    // OR initialize with a specific configuration file
    // EnigmaMachine machine("EnigmaMachineConfig1.toml", "./assets");

    // 2. Transform a character (0-25 index)
    int encrypted = machine.keyTransform(7); // 'H' -> ?
    
    return 0;
}
```

### Integration via CMake
If the library is installed on your system, you can find it using `find_package`:

```cmake
find_package(EnigmaMachineCore REQUIRED)
target_link_libraries(your_app PRIVATE EnigmaMachineCore::EnigmaCore)
```

Refer to the [Library API Specification](docs/Library_API.md) for full interface details.

## Installation

You can install the library and CLI globally on your system:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
sudo cmake --install build
```

This will install:
- **Headers:** `/usr/local/include/EnigmaMachineCore/`
- **Library:** `/usr/local/lib/libEnigmaCore.so` (or `.dll`/`.dylib`)
- **CLI:** `/usr/local/bin/EnigmaMachineCore`
- **Assets:** `/usr/local/share/EnigmaMachineCore/assets/`

The installed CLI and Library are intelligent enough to find their assets in the system's `share/` directory if a local `assets/` folder is not present.


## Running Tests

The project uses **Google Test (GTest)** for unit testing and **CTest** for execution. By default, tests are disabled to keep the build fast. To run them, you must explicitly enable them:

```bash
# 1. Configure with tests enabled
cmake -DENIGMA_BUILD_TESTS=ON -S . -B build

# 2. Build and run
cmake --build build
cd build && ctest --output-on-failure
```

For a detailed overview of the testing infrastructure, VS Code integration, and how to add new tests, refer to the [Testing Guide](docs/Testing.md).

## Benchmarking

EnigmaMachineCore includes a high-resolution benchmarking suite using **Google Benchmark**. This is used to measure encryption throughput and component latency.

```bash
# 1. Configure with benchmarks enabled
cmake -DENIGMA_BUILD_BENCHMARKS=ON -S . -B build_bench

# 2. Build and run
cmake --build build_bench --target EnigmaBenchmark
cd build_bench/benchmarks && ./EnigmaBenchmark
```

For more details on performance metrics and benchmarking options, see [docs/Benchmarking.md](docs/Benchmarking.md).

## Documentation

If you have Doxygen installed, you can generate the project documentation.

**Using the modern CMake CLI:**
```bash
cmake --build build --target Enigma_doxygen
```

**Using the traditional Makefile approach:**
```bash
cd build
make doxygen     # Generate only the documentation
```

HTML documentation is generated in: `docs/doxygen-gen-files/html/index.html`

## Roadmap

For a detailed view of our future plans, architectural evolution, and platform support strategy (including WASM, Android, and Embedded targets), please refer to our [Project Roadmap](roadmap.md).

Significant design choices and their rationales are documented in our [Architectural Decision Records (ADRs)](docs/adr/ADRs.md).

## Development

### Code Formatting
This project uses `clang-format` to maintain a consistent coding style. To format all source files:

```bash
cmake --build build --target enigma_format
```

### Static Analysis
The project integrates `clang-tidy` to detect bugs and enforce modern C++ practices. This check is optional and can be enabled during configuration:

```bash
cmake -DENIGMA_ENABLE_CLANG_TIDY=ON ..
```

## Contributing

We welcome contributions! Please see our [Contributing Guide](CONTRIBUTING.md) for details on:
- Our branching strategy (`main`, `development`, `feature/*`).
- How to submit Pull Requests using our standardized template.
- Coding standards and testing requirements.

## Customization

Rotors and Reflectors can be created and customized. Use the `assets` directory to place your customized components.

Alternatively, you can specify a custom assets directory at runtime using the `--assets` command-line option.

***Note:** The build system automatically copies the `assets/` folder to the binary directory after each build, ensuring the executable always has access to the latest configuration files.*

## License

[Apache-2.0](LICENSE)

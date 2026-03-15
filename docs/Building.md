# Building and Testing EnigmaMachineCore

This document outlines the build system architecture, prerequisites, and instructions for building and testing the EnigmaMachineCore project. For specific details on unit testing, see the [Testing Guide](Testing.md).

## Prerequisites & Dependencies

To build and run this project, you will need the following tools and libraries:

### Core Build Tools
*   **C++ Compiler:** Requires a C++20 compatible compiler (GCC 10+, Clang 10+, or MSVC 2019+).
*   **[CMake](https://cmake.org/):** Version 3.15 or higher is required to generate the build files.
*   **[GDB](https://www.sourceware.org/gdb/):** Essential for debugging logic on Linux environments.
*   **Git:** Required for version control and to manage the project's submodules.
*   **[clang-format](https://clang.llvm.org/docs/ClangFormat.html):** Recommended for maintaining consistent code style.
*   **[clang-tidy](https://clang.llvm.org/extra/clang-tidy/):** Used for static analysis during the build process.
*   **[cppcheck](http://cppcheck.sourceforge.net/):** Used for deeper static analysis, specifically performance, style, and portability.
*   **[Valgrind](https://valgrind.org/):** Essential for memory leak detection and profiling on Linux.
*   **[LLVM/Clang Sanitizers](https://clang.llvm.org/docs/index.html):** Used for runtime analysis (ASan, UBSan, MSan). Requires `clang` and `llvm` (for `llvm-symbolizer`).
*   **[lcov](https://ltp.sourceforge.net/coverage/lcov.php):** Code coverage reporting tool. Used with gcov for coverage analysis.

### Libraries
*   **[toml11](https://github.com/ToruNiina/toml11):** A powerful C++11 header-only library for TOML.
*   **[CLI11](https://github.com/CLIUtils/CLI11):** Command line parser for C++11.

*Note: These are included as git submodules in the `external/` directory.*

### Testing & Benchmarking Tools
*   **[GoogleTest (gTest)](https://github.com/google/googletest):** Used for unit testing the core logic.
*   **[Google Benchmark](https://github.com/google/benchmark):** Used for performance profiling and throughput measurement.
*   **[RapidCheck](https://github.com/emil-e/rapidcheck):** Property-based testing for verifying cryptographic properties (reciprocity). Automatically downloaded via CMake FetchContent when `ENIGMA_BUILD_PROPERTY_TESTS=ON`.

*Note: You do NOT need to install these libraries on your machine. The build system (CMake) automatically downloads and configures them during the first build (requires internet connection for the initial configuration).*

### Documentation & Visualization (Optional)
*   **[Doxygen](https://www.doxygen.nl/):** Used for generating the HTML/XML API documentation.
*   **[Graphviz](https://graphviz.org/):** Enables Doxygen to generate interactive call graphs and class diagrams.
*   **[PlantUML](https://plantuml.com/):** Required if you wish to process and view manual diagrams within the `/docs` folder.

---

## Build System Architecture

To support robust testing and modularity, the project uses a "Library + Runner" architecture.

### The Components

1.  **EnigmaCore (Shared/Static Library):**
    *   **Content:** The final distributable binary. It contains only the **Public API** symbols.
    *   **Visibility:** Symbols are hidden by default to ensure binary stability. Only interfaces annotated with `ENIGMACORE_EXPORT` are visible.
    *   **Purpose:** The production-ready engine for external applications.
    *   **Library Type:** Generic (respects `BUILD_SHARED_LIBS`).

2.  **EnigmaCore_OBJ (Object Library):**
    *   **Content:** Contains ALL symbols (public + internal) as unlinked object files.
    *   **Purpose:** Acts as the "source of truth" for both the production library and the test suite. This allows tests to access internal components (`Rotor`, `PlugBoard`) without exposing them in the final `libEnigmaCore.so`.

3.  **EnigmaMachineCore (Application Executable):**
    *   **Content:** Contains only `app/main.cpp`.
    *   **Purpose:** The user-facing application. It links to `EnigmaCore` to do the actual work.
    *   **CMake Variable:** `${PROJECT_NAME}`

4.  **EnigmaTests (Test Executable):**
    *   **Content:** Contains the GoogleTest runner (`gtest_main`) and all unit test files.
    *   **Purpose:** Runs the test suite. It also links to `EnigmaCore_OBJ` to test the logic directly.

### Architecture Visualization

```text
       ${INCLUDE_DIRS} (Headers)
                |
                v
       +-----------------+
       | ${CORE_SOURCES} |  <-- Logic (Rotor, PlugBoard...)
       +-----------------+
                |
      Creates "EnigmaCore_OBJ" (Internal)
                |
      +---------+----------+
      |                    |
      v                    v
"EnigmaCore" Library   EnigmaTests
 (Public API only)    (Full Access)
      |                    |
      v                    v
EnigmaMachineCore     [ Test Results ]
  (Executable)
```

## CMake Options & Variables

The project uses CMake options to control the build process. By default, **only the core library and CLI application are built** to ensure maximum build speed.

### Build Toggles
*   **`BUILD_SHARED_LIBS`**: If `ON`, builds `EnigmaCore` as a shared library (`.so`, `.dll`). If `OFF` (default), builds it as a static library.
*   **`ENIGMA_BUILD_CLI`**: Build the main Enigma machine command-line tool. Default is `ON`.
*   **`ENIGMA_BUILD_TESTS`**: Build the GoogleTest-based unit test suite. Default is `OFF`.
*   **`ENIGMA_BUILD_BENCHMARKS`**: Build the Google Benchmark-based performance suite. Default is `OFF`.
*   **`ENIGMA_BUILD_DOCS`**: Build the Doxygen documentation targets. Default is `OFF`.
*   **`ENIGMA_BUILD_PROPERTY_TESTS`**: Build the RapidCheck-based property-based test suite. Default is `OFF`.

### Feature Options
*   **`ENIGMA_ENABLE_CLANG_TIDY`**: Enable static analysis during the build process. Default is `OFF`.
*   **`ENIGMA_ENABLE_COVERAGE`**: Enable code coverage instrumentation (gcov/lcov). Default is `OFF`.
*   **`ENIGMA_ENABLE_CPPCHECK`**: Run cppcheck static analysis. Default is `OFF`.

### Internal Variables
*   **`${PROJECT_NAME}`**: The name of the main project (`EnigmaMachineCore`).
*   **`${CORE_SOURCES}`**: The list of `.cpp` files compiled into the `EnigmaCore` library.
*   **`${INCLUDE_DIRS}`**: Directory paths added to `EnigmaCore` with `PUBLIC` visibility.

---

## Build Instructions

The project supports different build types. It is recommended to use separate directories for Debug and Release builds.

### Release Build (Optimized for performance)
```bash
cmake -DCMAKE_BUILD_TYPE=Release -S . -B build/release
cmake --build build/release
```

### Debug Build (With debug symbols)
```bash
cmake -DCMAKE_BUILD_TYPE=Debug -S . -B build/debug
cmake --build build/debug
```

### Running Tests
To run the GoogleTest suite (works with either build type, provided it was configured):
```bash
cd build/debug # or build/release
ctest --output-on-failure
```

### Running the Application
```bash
# Basic usage (Round-trip: Encode then Decode)
./build/release/EnigmaMachineCore -m "SECRET"

# Encode only
./build/release/EnigmaMachineCore -m "SECRET" --encode

# Decode only (Symmetric operation)
./build/release/EnigmaMachineCore -m "NMRGDU" --decode

# Custom assets directory and Debug output
./build/release/EnigmaMachineCore -m "HELLO" --assets ./my_assets --debug
```

*Note: The application first looks for assets in a local `assets/` directory. If it doesn't find them, it automatically falls back to the system's installation path (e.g., `/usr/local/share/EnigmaMachineCore/assets/`).*

---

## Installation & Distribution

To install the project globally on your system:

```bash
# 1. Configure and Build
cmake -DCMAKE_BUILD_TYPE=Release -S . -B build/release
cmake --build build/release

# 2. Install
# Default prefix is /usr/local on Linux/macOS or C:\Program Files on Windows
sudo cmake --install build/release
```

### Consuming the Library (find_package)
Once installed, your own C++ projects can easily integrate the library:

```cmake
# CMakeLists.txt
find_package(EnigmaMachineCore REQUIRED)
add_executable(my_app main.cpp)
target_link_libraries(my_app PRIVATE EnigmaMachineCore::EnigmaCore)
```

```cpp
// main.cpp
#include <EnigmaMachineCore/EnigmaCore.hpp>
```

---

## Code Formatting

To ensure a consistent style, the project uses `clang-format` (version 21 or higher) based on the Google C++ style guide. Using different versions of `clang-format` may result in slight formatting discrepancies that could fail CI checks.

### To format the source code:
```bash
cmake --build build --target enigma_format
```

This will automatically format all source and header files in the project.

---

## Static Analysis

Static analysis is performed using **clang-tidy** and **cppcheck**, both of which are integrated into the CMake build system.

### Clang-Tidy
*   **How to Enable:** By default, static analysis is disabled to ensure fast build times. To enable it, use the `ENIGMA_ENABLE_CLANG_TIDY` flag during configuration:
    ```bash
    cmake -DENIGMA_ENABLE_CLANG_TIDY=ON -S . -B build
    ```
*   **Execution:** Once enabled, clang-tidy will run on every source file during compilation (`make`).
*   **Identifying Issues:** Clang-tidy output is interleaved with compiler output. You can distinguish them by the bracketed check name at the end of the line (e.g., `[modernize-use-auto]`). Standard compiler warnings typically start with `-W`.
*   **Configuration:** The list of active checks is defined in `CMakeLists.txt`.

### CppCheck
*   **How to Enable:** Use the `ENIGMA_ENABLE_CPPCHECK` flag during configuration:
    ```bash
    cmake -DENIGMA_ENABLE_CPPCHECK=ON -S . -B build
    ```
*   **Execution:** Unlike clang-tidy, cppcheck is run via a dedicated custom target:
    ```bash
    cmake --build build --target enigma_cppcheck
    ```
*   **Purpose:** It focuses on performance, portability, style, and unused functions.
*   **CI Enforcement:** In the GitHub Actions pipeline, `enigma_cppcheck` is run on every push/PR within the `Code Analysis` workflow.

---

## Sanitizers (Runtime Analysis)

The project supports LLVM/Clang Sanitizers to detect memory errors, uninitialized reads, and undefined behavior at runtime.

### Requirements
*   **Compiler:** Requires `clang` / `clang++`.
*   **Tools:** `llvm` (specifically `llvm-symbolizer`) is recommended for descriptive backtraces.

### Available Options
| Option | Sanitizer |
| :--- | :--- |
| `ENIGMA_USE_ASAN` | AddressSanitizer (ASan) |
| `ENIGMA_USE_UBSAN` | UndefinedBehaviorSanitizer (UBSan) |
| `ENIGMA_USE_MSAN` | MemorySanitizer (MSan) |

### Usage
1.  **Configure with Clang and Sanitizers:**
    ```bash
    cmake -DCMAKE_CXX_COMPILER=clang++ -DENIGMA_USE_ASAN=ON -S . -B build
    ```
2.  **Build and Run:**
    ```bash
    cmake --build build
    ./scripts/run_sanitizers.sh build
    ```

*Note: Sanitizers are automatically excluded from the Unit Test suite to maintain performance. They are applied to the core engine and CLI application. In CI, these checks are centralized in the `code-analysis.yml` workflow.*

---

## Generating Documentation

If you have Doxygen and PlantUML installed, you can generate the project's API documentation and architectural diagrams locally.

### To generate the documentation:

```bash
cmake --build build --target Enigma_doxygen
```

This command will:
1.  **Generate Diagrams:** Use PlantUML to process `.puml` files in `docs/diagrams/` and output SVG images to `docs/diagrams/output/`.
2.  **Generate HTML/XML:** Use Doxygen to parse source code and Markdown files to create a complete documentation site.

### Output Location

The generated HTML documentation can be found at:
`docs/doxygen-gen-files/html/index.html`

Open this file in any web browser to view the interactive documentation.

## VS Code Integration

The project includes pre-configured settings for Visual Studio Code to streamline development and testing:

### Building & Running
*   **Build:** Press `Ctrl+Shift+B` to trigger the default build (**Debug**).
*   **Debug App:** Press `F5` while the **(gdb) Launch (Debug)** configuration is selected in the Run sidebar.
*   **Tasks:** Access `Terminal > Run Task...` for specific commands like `Build (Release)` or `CMake: Configure`.

### Testing
*   **Run All Tests:** Access `Terminal > Run Task...` and select **Run Tests**. This executes `ctest` and displays output in the terminal.
*   **Debug Tests:** Open the **Run and Debug** sidebar (Ctrl+Shift+D), select **(gdb) Launch Tests** from the dropdown, and press `F5`. This allows you to set breakpoints in your test files.
*   **Test Explorer:** If you have the **CMake Tools** extension installed, you can use the Test Explorer (beaker icon) to run or debug individual tests.

---

## Submodule Support

This project is designed to be easily embedded as a submodule in other CMake projects.

*   **Default Behavior:** When included via `add_subdirectory()`, the build system automatically detects it is not the top-level project and **disables** the CLI executable, tests, and documentation targets to keep your build clean.
*   **Configuration Options:** You can override this behavior by setting these variables before adding the subdirectory:

    ```cmake
    set(ENIGMA_BUILD_CLI ON)   # Build the CLI executable
    set(ENIGMA_BUILD_TESTS ON) # Build the test suite
    set(ENIGMA_BUILD_DOCS ON)  # Build documentation targets
    add_subdirectory(external/EnigmaMachineCore)
    ```

*   **Integration:** Link against the `EnigmaCore` library and include the public header:
    ```cmake
    target_link_libraries(MyTarget PRIVATE EnigmaMachineCore::EnigmaCore)
    ```
    ```cpp
    #include <EnigmaMachineCore/EnigmaCore.hpp>
    ```

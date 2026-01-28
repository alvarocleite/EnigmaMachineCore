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

### Libraries
*   **[toml11](https://github.com/ToruNiina/toml11):** A powerful C++11 header-only library for TOML.
*   **[CLI11](https://github.com/CLIUtils/CLI11):** Command line parser for C++11. 
  
*Note: These are included as git submodules in the `external/` directory.*

### Testing Tools
*   **[GoogleTest (gTest)](https://github.com/google/googletest):** Used for unit testing the core logic.

*Note: You do NOT need to install GoogleTest on your machine. The build system (CMake) automatically downloads and configures it during the first build (requires internet connection for the initial configuration).*

### Documentation & Visualization (Optional)
*   **[Doxygen](https://www.doxygen.nl/):** Used for generating the HTML/XML API documentation.
*   **[Graphviz](https://graphviz.org/):** Enables Doxygen to generate interactive call graphs and class diagrams.
*   **[PlantUML](https://plantuml.com/):** Required if you wish to process and view manual diagrams within the `/docs` folder.

---

## Build System Architecture

To support robust testing and modularity, the project uses a "Library + Runner" architecture.

### The Components

1.  **EnigmaCore (Static Library):**
    *   **Content:** Contains all the business logic (`EnigmaMachine`, `Rotor`, `PlugBoard`, etc.).
    *   **Purpose:** This is the "engine". It has NO `main()` function. This allows it to be linked into multiple different executables (like the main app or the test suite).
    *   **CMake Target:** `EnigmaCore`

2.  **EnigmaMachineCore (Application Executable):**
    *   **Content:** Contains only `app/main.cpp`.
    *   **Purpose:** The user-facing application. It links to `EnigmaCore` to do the actual work.
    *   **CMake Variable:** `${PROJECT_NAME}`

3.  **EnigmaTests (Test Executable):**
    *   **Content:** Contains the GoogleTest runner (`gtest_main`) and all unit test files.
    *   **Purpose:** Runs the test suite. It also links to `EnigmaCore` to test the logic directly.

### Architecture Visualization

```text
       ${INCLUDE_DIRS} (Headers)
                |
                v
       +-----------------+
       | ${CORE_SOURCES} |  <-- Logic (Rotor, PlugBoard...)
       +-----------------+
                |
      Creates "EnigmaCore" Library
                |
      +---------+----------+
      |                    |
      v                    v
${APP_SOURCES}      [ Test Sources ]
 (app/main.cpp)      (BasicTest.cpp)
      |                    |
      v                    v
${PROJECT_NAME}       EnigmaTests
 (Executable)         (Executable)
```

## CMake Variables

*   **`${PROJECT_NAME}`**: The name of the main project (`EnigmaMachineCore`). Used to name the final application binary.
*   **`${CORE_SOURCES}`**: The list of `.cpp` files that constitute the Enigma logic (e.g., `EnigmaMachine.cpp`, `Rotor.cpp`). These are compiled into the `EnigmaCore` library.
*   **`${APP_SOURCES}`**: The list containing *only* the application entry point (`app/main.cpp`).
*   **`${INCLUDE_DIRS}`**: A list of directory paths containing header files. These are added to the `EnigmaCore` library with `PUBLIC` visibility. This means any target that links `EnigmaCore` (like the App or Tests) automatically gets these include paths, simplifying configuration.

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

*Note: By default, the application looks for assets in the `assets/` directory relative to the executable (automatically copied by the build system).*

---

## Code Formatting

To ensure a consistent style, the project uses `clang-format` based on the Google C++ style guide. 

### To format the source code:
```bash
cmake --build build --target enigma_format
```

This will automatically format all source and header files in the project.

---

## Static Analysis

Static analysis is performed using **clang-tidy** and is integrated into the CMake build system.

*   **How to Enable:** By default, static analysis is disabled to ensure fast build times. To enable it, use the `ENIGMA_ENABLE_CLANG_TIDY` flag during configuration:
    ```bash
    cmake -DENIGMA_ENABLE_CLANG_TIDY=ON -S . -B build
    ```
*   **Execution:** Once enabled, clang-tidy will run on every source file during compilation (`make`).
*   **Identifying Issues:** Clang-tidy output is interleaved with compiler output. You can distinguish them by the bracketed check name at the end of the line (e.g., `[modernize-use-auto]`). Standard compiler warnings typically start with `-W`.
*   **Configuration:** The list of active checks is defined in `CMakeLists.txt`.

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
    target_link_libraries(MyTarget PRIVATE EnigmaCore)
    ```
    ```cpp
    #include <EnigmaCore.hpp>
    ```

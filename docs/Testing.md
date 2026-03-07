# Testing Guide

This project uses **GoogleTest (gTest)** for unit testing and **CTest** as the test runner. 

## Testing Strategy

The project follows a "centralized mirror" strategy:
*   All tests are located in the root `tests/` directory.
*   The structure inside `tests/` strictly mirrors the structure of the project's source code.
*   Tests link against the `EnigmaCore` static library, allowing them to test the logic without the application's `main()` function interfering.

## Directory Structure

```text
tests/
├── CMakeLists.txt
├── AssetProvider/
│   ├── TestFileAssetProvider.cpp
│   └── TestSpec.md
├── EnigmaMachine/
│   ├── TestEnigmaConfigLoader.cpp
│   ├── TestEnigmaMachine.cpp
│   ├── TestSpec.md
│   └── TestSpecLoader.md
├── PlugBoard/
│   ├── TestPlugBoard.cpp
│   └── TestSpec.md
└── RotorBox/
    ├── TestReflector.cpp
    ├── TestReflectorSpec.md
    ├── TestRotor.cpp
    ├── TestRotorBox.cpp
    ├── TestRotorSpec.md
    ├── TestSpec.md
    ├── TestTransformer.cpp
    └── TestTransformerSpec.md
```

## How to Run Tests

### Configuration
By default, tests are **disabled** to maintain fast build times for the core application. You must explicitly enable them during the CMake configuration step:

```bash
cmake -DENIGMA_BUILD_TESTS=ON -S . -B build
```

### Command Line (CLI)
Navigate to your build directory (usually `build/debug`) and use `ctest`:

```bash
cd build/debug
ctest --output-on-failure
```

### Visual Studio Code
1.  **Run Task:** Press `Ctrl+Shift+P` -> `Run Task` -> **Run Tests**.
2.  **Debug:** Open the **Run and Debug** sidebar, select **(gdb) Launch Tests**, and press `F5`.
3.  **Test Explorer:** Use the beaker icon in the sidebar (requires CMake Tools extension).

## Adding New Tests

### 1. Create the Test File
Add a new `.cpp` file in the appropriate subdirectory within `tests/`. Use the GoogleTest macros:

```cpp
#include <gtest/gtest.h>
#include "YourHeader.hpp"

TEST(YourModuleTests, FeatureDescription) {
    // Arrange
    // Act
    // Assert
    EXPECT_EQ(1, 1);
}
```

### 2. Register the File
Open `tests/CMakeLists.txt` and add your new file to the `add_executable(EnigmaTests ...)` list:

```cmake
add_executable(EnigmaTests
    ...
    YourModule/YourNewTest.cpp
)
```

### 3. Build and Run
Re-run your build command. CMake will automatically detect the new tests.

## Sanitizers (Clang)

To improve code reliability and detect memory errors or undefined behavior, the project integrates LLVM/Clang Sanitizers.

### Requirements
*   **Compiler:** `clang` / `clang++` must be used for sanitizer builds.

### Enabling Sanitizers
You can enable sanitizers individually during the CMake configuration step:

| Option | Sanitizer | Description |
| :--- | :--- | :--- |
| `ENIGMA_USE_ASAN` | AddressSanitizer (ASan) | Detects heap/stack buffer overflows, use-after-free, and memory leaks. |
| `ENIGMA_USE_UBSAN` | UndefinedBehaviorSanitizer (UBSan) | Detects signed integer overflow, null pointer dereference, and other C++ undefined behaviors. |
| `ENIGMA_USE_MSAN` | MemorySanitizer (MSan) | Detects uninitialized memory reads. |

**Example Command:**
```bash
cmake -DCMAKE_CXX_COMPILER=clang++ -DENIGMA_USE_ASAN=ON -B build
cmake --build build
```

### Local Verification
To run the same scenarios as the CI locally, use the provided helper script:
```bash
# Ensure you have built the CLI with sanitizers enabled
./scripts/run_sanitizers.sh build
```

### Constraints
*   **Exclusion of Tests:** By design, sanitizers are applied to the core engine and the CLI application but are **excluded** from the Unit Test suite targets. This is achieved by using a separate non-instrumented object library for tests when sanitizers are active, reducing noise and execution time for the test suite.
*   **Performance:** Sanitizers introduce runtime overhead (typically 2x-4x) and should be used during development or in specific CI pipelines rather than production builds.

### CI Integration
Code analysis is centralized in the **Code Analysis** workflow (`code-analysis.yml`), which runs on every push and pull request. It orchestrates parallel jobs for:
*   **Static Analysis:** Running Clang-Tidy on the core logic.
*   **Runtime Sanitizers:** Executing ASan, UBSan, and MSan checks via the reusable `sanitizers.yml` workflow.

Results are aggregated into a single **GitHub Job Summary** without failing the build, allowing the team to monitor and fix issues asynchronously.

## GoogleTest Dependency
The project uses CMake's `FetchContent` to download GoogleTest automatically.
*   **Offline Mode:** You must run the initial CMake configuration while online. After that, the library is stored in `build/<build_type>/_deps` and can be used offline.
*   **Installation:** No manual installation of GoogleTest is required on your system.

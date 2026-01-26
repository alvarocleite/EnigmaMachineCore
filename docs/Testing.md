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

## GoogleTest Dependency
The project uses CMake's `FetchContent` to download GoogleTest automatically.
*   **Offline Mode:** You must run the initial CMake configuration while online. After that, the library is stored in `build/<build_type>/_deps` and can be used offline.
*   **Installation:** No manual installation of GoogleTest is required on your system.

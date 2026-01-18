# Test Specification: AssetProvider

## Overview
This specification covers the unit tests for the `IAssetProvider` interface and its concrete implementation, `FileAssetProvider`. The goal is to ensure that assets can be correctly loaded from the filesystem and that appropriate errors are raised for invalid inputs.

## Tested Components
*   `IAssetProvider` (Interface)
*   `FileAssetProvider` (Concrete Implementation)

## Test Cases

### 1. FileAssetProvider: Load Existing File
**Objective:** Verify that `FileAssetProvider` can successfully open and read the content of a valid file.
**Preconditions:** A known file exists (e.g., `assets/Rotor1.toml`).
**Steps:**
1.  Initialize `FileAssetProvider`.
2.  Call `loadAsset()` with the path to the existing file.
3.  Verify that the returned string is not empty.
4.  Verify that the content matches the expected file header/content (e.g., starts with `[rotor]`).

### 2. FileAssetProvider: Load Non-Existent File
**Objective:** Verify that `FileAssetProvider` correctly handles requests for missing files.
**Preconditions:** None.
**Steps:**
1.  Initialize `FileAssetProvider`.
2.  Call `loadAsset()` with a path to a non-existent file (e.g., `assets/ghost_rotor.toml`).
3.  **Expected Result:** The method throws a `std::runtime_error`.

### 3. FileAssetProvider: Load Binary File (Optional/Edge Case)
**Objective:** Verify that `FileAssetProvider` can handle binary or non-text files without crashing (though usage is primarily text).
**Preconditions:** A binary file exists (or create a dummy one).
**Steps:**
1.  Initialize `FileAssetProvider`.
2.  Load the binary file.
3.  **Expected Result:** Content is returned as a `std::string` containing the binary data.

## MockAssetProvider (For future integration tests)
*Note: While not strictly a unit test of FileAssetProvider, creating a MockAssetProvider demonstrates the utility of the interface.*
**Objective:** Verify that a mock provider can simulate file content without disk access.
**Steps:**
1.  Define a `MockAssetProvider` implementing `IAssetProvider`.
2.  Pre-load it with a virtual file "virtual_rotor.toml" mapping to specific content.
3.  Call `loadAsset("virtual_rotor.toml")`.
4.  **Expected Result:** Returns the pre-loaded content.

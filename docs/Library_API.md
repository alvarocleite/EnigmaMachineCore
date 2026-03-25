# EnigmaCore Library: Public API Specification

This document defines the official public interface of the **EnigmaCore** library. Following the "Strict Facade" architectural pattern, the library exposes only the minimal set of interfaces required to configure, execute, and monitor the Enigma engine.

---

## 1. Architectural Strategy: Strict Facade

To ensure long-term binary stability (ABI) and security, EnigmaCore enforces a clean separation between the **Public Surface** and **Internal Mechanics**.

*   **Encapsulation:** All internal components (`Rotor`, `PlugBoard`, `RotorBox`) are hidden using forward declarations and the Pimpl (Pointer to Implementation) pattern.
*   **Symbol Visibility:** Only classes and methods explicitly annotated with `ENIGMACORE_EXPORT` are visible in the shared library's export table.
*   **Strong Typing:** The signal path uses semantic types (e.g., `AlphabetIndex`) instead of raw primitives to ensure range validation and architectural clarity.
*   **Decoupling:** High-level interfaces (`IAssetProvider`, `IEnigmaObserver`, `ILogger`) allow consumers to extend the library's behavior without accessing its private state.

---

## 2. Common Types: `EnigmaTypes.hpp`

This header defines the basic types used across the library to ensure type safety and platform independence.

```cpp
/** @brief Semantic type for alphabet indexes (0 to TRANSFORMER_SIZE - 1). */
using AlphabetIndex = int;

/** @brief Severity levels for logging. */
enum class LogLevel { Debug, Info, Warning, Error };

/** @brief Interface for platform-specific logging. */
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(LogLevel level, std::string_view message) = 0;
};
```

---

## 3. Core Execution: `EnigmaMachine`

The `EnigmaMachine` class is the primary entry point for all encryption operations.

### **Constructors**
*   `EnigmaMachine()`: Initializes a standard machine with default rotors (I, II, III). If the library is installed, it automatically attempts to find these assets in the system's `share/` directory.
*   `EnigmaMachine(string_view fileName, string_view assetPath)`: Standard file-based initialization.
*   `EnigmaMachine(IAssetProvider& provider, string_view fileName, string_view assetPath)`: Dependency-injected initialization for custom environments.

### **Static Factory Methods** (Recommended)
*   `enigma::Result<EnigmaMachine> create(string_view fileName, string_view assetPath)`: Creates a machine from a config file. Returns `Result<EnigmaMachine>` - use `has_value()` to check success, `error()` to get failure reason.
*   `enigma::Result<EnigmaMachine> create(IAssetProvider& provider, string_view fileName, string_view assetPath)`: Creates a machine using a custom asset provider. Returns `Result<EnigmaMachine>` for error-safe initialization.

### **Primary Methods**
*   `AlphabetIndex keyTransform(AlphabetIndex input)`: Transforms a single character index (0 to `TRANSFORMER_SIZE - 1`). This method handles the internal rotor stepping, multi-stage transformation, and observer notifications.
*   `void processBuffer(std::span<AlphabetIndex> buffer)`: Processes a contiguous block of characters in-place. This is the preferred method for performance-critical batch processing.
*   `void setLogger(ILogger* logger)`: Attaches a custom logger implementation to the engine.
*   `void registerObserver(IEnigmaObserver*)`: Attaches a listener to monitor internal events.
*   `void removeObserver(IEnigmaObserver*)`: Detaches a listener.

---

## 4. Monitoring: `IEnigmaObserver`

Consumers implement this interface to react to state changes within the machine.

```cpp
struct IEnigmaObserver {
    virtual ~IEnigmaObserver();

    // Triggered when a rotor moves (0 to TRANSFORMER_SIZE - 1)
    virtual void onRotorStepped(int rotorIndex, AlphabetIndex position) = 0;

    // Triggered after each character is processed
    virtual void onCharEncrypted(char input, char output) = 0;
};
```


---

## 4. Extensibility: `IAssetProvider`

This interface decouples the engine from the physical filesystem, enabling support for WebAssembly (Memory-based) and Embedded (Flash-based) targets.

```cpp
class IAssetProvider {
public:
    virtual ~IAssetProvider();

    // Loads the raw string content of a configuration or wiring asset
    // Returns enigma::Result<std::string> - use has_value() to check success
    virtual enigma::Result<std::string> loadAsset(std::string_view assetName) const = 0;
};
```

---

## 5. Usage Example

```cpp
#include <EnigmaMachineCore/EnigmaCore.hpp>
#include <iostream>

class MyLogger : public IEnigmaObserver {
    void onRotorStepped(int idx, AlphabetIndex pos) override {
        std::cout << "Rotor " << idx << " moved to " << pos << "\n";
    }
    void onCharEncrypted(char in, char out) override {}
};

int main() {
    // 1a. Initialize with factory method (recommended - returns Result)
    auto result = EnigmaMachine::create("config.toml", "assets/");
    if (!result) {
        std::cerr << "Failed to create machine: " << static_cast<int>(result.error()) << "\n";
        return 1;
    }
    EnigmaMachine machine = std::move(*result);

    // 1b. Or use constructor directly (throws on failure)
    // EnigmaMachine machine("config.toml", "assets/");

    // 2. Observe
    MyLogger logger;
    machine.registerObserver(&logger);

    // 3. Execute (Single character)
    AlphabetIndex result_char = machine.keyTransform(7); // 'H'

    // 4. Execute (Batch processing)
    std::vector<AlphabetIndex> buffer = {0, 1, 2}; // "ABC"
    machine.processBuffer(buffer);

    return 0;
}
```

# EnigmaCore Library: Public API Specification

This document defines the official public interface of the **EnigmaCore** library. Following the "Strict Facade" architectural pattern, the library exposes only the minimal set of interfaces required to configure, execute, and monitor the Enigma engine.

---

## 1. Architectural Strategy: Strict Facade

To ensure long-term binary stability (ABI) and security, EnigmaCore enforces a clean separation between the **Public Surface** and **Internal Mechanics**.

*   **Encapsulation:** All internal components (`Rotor`, `PlugBoard`, `RotorBox`) are hidden using forward declarations and the Pimpl (Pointer to Implementation) pattern.
*   **Symbol Visibility:** Only classes and methods explicitly annotated with `ENIGMACORE_EXPORT` are visible in the shared library's export table.
*   **Decoupling:** High-level interfaces (`IAssetProvider`, `IEnigmaObserver`) allow consumers to extend the library's behavior without accessing its private state.

---

## 2. Core Execution: `EnigmaMachine`

The `EnigmaMachine` class is the primary entry point for all encryption operations.

### **Constructors**
*   `EnigmaMachine()`: Initializes a standard machine with default rotors (I, II, III) and no plugboard swaps.
*   `EnigmaMachine(string_view fileName, string_view assetPath)`: Standard file-based initialization.
*   `EnigmaMachine(IAssetProvider& provider, string_view fileName, string_view assetPath)`: Dependency-injected initialization for custom environments.

### **Primary Methods**
*   `int keyTransform(int input)`: Transforms a single character index (0-25). This method handles the internal rotor stepping, multi-stage transformation, and observer notifications.
*   `void registerObserver(IEnigmaObserver*)`: Attaches a listener to monitor internal events.
*   `void removeObserver(IEnigmaObserver*)`: Detaches a listener.

---

## 3. Monitoring: `IEnigmaObserver`

Consumers implement this interface to react to state changes within the machine without having direct access to the rotors.

```cpp
struct IEnigmaObserver {
    virtual ~IEnigmaObserver();

    // Triggered when a rotor moves (0-25)
    virtual void onRotorStepped(int rotorIndex, int position) = 0;

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
    virtual std::string loadAsset(std::string_view assetName) = 0;
};
```

---

## 5. Usage Example

```cpp
#include <EnigmaCore.hpp>
#include <iostream>

class MyLogger : public IEnigmaObserver {
    void onRotorStepped(int idx, int pos) override { 
        std::cout << "Rotor " << idx << " moved to " << pos << "
"; 
    }
    void onCharEncrypted(char in, char out) override {}
};

int main() {
    // 1. Initialize
    EnigmaMachine machine("config.toml", "./assets");
    
    // 2. Observe
    MyLogger logger;
    machine.registerObserver(&logger);
    
    // 3. Execute
    int result = machine.keyTransform(7); // 'H'
    
    return 0;
}
```

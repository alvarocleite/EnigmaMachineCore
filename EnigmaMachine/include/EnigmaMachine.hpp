/**
 * @file
 * @brief Header file for the EnigmaMachine class.
 */

#pragma once

#include <memory>
#include <string_view>
#include <vector>

#include "EnigmaCore_EXPORT.hpp"
#include "IEnigmaObserver.hpp"

// Forward declarations for internal implementation details
class RotorBox;
class PlugBoard;
struct EnigmaMachineConfig;
class IAssetProvider;

/**
 * @brief Class representing the Enigma machine.
 * This class encapsulates the functionality of the Enigma machine, providing a simple
 * interface for encryption while hiding the complexity of the rotors and plugboard.
 */
class ENIGMACORE_EXPORT EnigmaMachine : public IEnigmaObserver {
protected:
    /**
     * @brief Internal constructor using the configuration struct.
     * Accessible to tests and benchmarks, but hidden from the public API.
     */
    explicit EnigmaMachine(const EnigmaMachineConfig& config);

private:
    std::unique_ptr<RotorBox> rotorBox;
    std::unique_ptr<PlugBoard> plugBoard;
    std::vector<IEnigmaObserver*> observers;

public:
    /**
     * @brief Default Constructor.
     * Initializes a standard Enigma Machine (3 Rotors, standard Reflector).
     */
    EnigmaMachine();

    /**
     * @brief File-based Constructor using a specific Asset Provider.
     *
     * @param provider The asset provider to use for loading configuration.
     * @param fileName The path to the TOML configuration file.
     * @param assetPath Optional base directory for assets.
     */
    EnigmaMachine(IAssetProvider& provider, std::string_view fileName, std::string_view assetPath = "");

    /**
     * @brief File-based Constructor.
     * Initializes the machine by parsing a TOML configuration file from the filesystem.
     *
     * @param fileName The path to the TOML configuration file.
     * @param assetPath Optional base directory for assets.
     */
    EnigmaMachine(std::string_view fileName, std::string_view assetPath = "");

    // Rule of Five (Required due to unique_ptr)
    ~EnigmaMachine() override;
    EnigmaMachine(const EnigmaMachine&) = delete;
    EnigmaMachine& operator=(const EnigmaMachine&) = delete;
    EnigmaMachine(EnigmaMachine&& other) noexcept;
    EnigmaMachine& operator=(EnigmaMachine&& other) noexcept;

    /**
     * @brief Transforms the input key through the rotor box.
     *
     * @param input The input key to be transformed.
     * @return int The transformed output key.
     */
    int keyTransform(int input);

    /**
     * @brief Registers an observer to receive notifications.
     * @param observer The observer to register.
     */
    void registerObserver(IEnigmaObserver* observer);

    /**
     * @brief Removes an observer.
     * @param observer The observer to remove.
     */
    void removeObserver(IEnigmaObserver* observer);

    // IEnigmaObserver implementation
    void onRotorStepped(int rotorIndex, int position) override;
    void onCharEncrypted(char input, char output) override;
};
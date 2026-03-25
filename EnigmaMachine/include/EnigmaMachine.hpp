/**
 * @file
 * @brief Header file for the EnigmaMachine class.
 */

#pragma once

#include <memory>
#include <span>
#include <string_view>
#include <vector>

#include "EnigmaCore_EXPORT.hpp"
#include "EnigmaError.hpp"
#include "EnigmaTypes.hpp"
#include "IEnigmaObserver.hpp"

// Forward declarations for internal implementation details
class RotorBox;
class PlugBoard;
struct EnigmaMachineConfig;
class IAssetProvider;
namespace enigma {
struct EnigmaMachineData;
}

/**
 * @brief Class representing the Enigma machine.
 *
 * @image html Enigma_Signal_Flow_Diagram.svg "Complete Signal Flow" width=1000px
 *
 * This class encapsulates the functionality of the Enigma machine, providing a simple
 * interface for encryption while hiding the complexity of the rotors and plugboard.
 */
class ENIGMACORE_EXPORT EnigmaMachine : public IEnigmaObserver {
protected:
    /**
     * @brief Internal constructor using the configuration struct.
     * Accessible to tests and benchmarks, but hidden from the public API.
     * @param config The machine configuration.
     * @param logger Optional logger for event reporting.
     */
    explicit EnigmaMachine(const EnigmaMachineConfig& config, ILogger* logger = nullptr);

    /**
     * @brief Internal constructor using the configuration struct (move version).
     * @param config The machine configuration.
     * @param logger Optional logger for event reporting.
     */
    explicit EnigmaMachine(EnigmaMachineConfig&& config, ILogger* logger = nullptr);

protected:
    std::unique_ptr<RotorBox> rotorBox;
    std::unique_ptr<PlugBoard> plugBoard;
    std::vector<IEnigmaObserver*> observers;
    ILogger* logger = nullptr;

public:
    /**
     * @brief Default Constructor.
     * Initializes a standard Enigma Machine (3 Rotors, standard Reflector).
     * @param logger Optional logger for event reporting.
     */
    explicit EnigmaMachine(ILogger* logger = nullptr);

    /**
     * @brief File-based Constructor using a specific Asset Provider.
     *
     * @param provider The asset provider to use for loading configuration.
     * @param fileName The path to the TOML configuration file.
     * @param assetPath Optional base directory for assets.
     * @param logger Optional logger for event reporting.
     */
    EnigmaMachine(const IAssetProvider& provider, std::string_view fileName, std::string_view assetPath = "",
                  ILogger* logger = nullptr);

    /**
     * @brief Constructor using POD DTO for embedded/WASM targets.
     * @param data The POD configuration data.
     * @param logger Optional logger for event reporting.
     */
    explicit EnigmaMachine(const enigma::EnigmaMachineData& data, ILogger* logger = nullptr);

    /**
     * @brief File-based Constructor.
     * Initializes the machine by parsing a TOML configuration file from the filesystem.
     *
     * @param fileName The path to the TOML configuration file.
     * @param assetPath Optional base directory for assets.
     * @param logger Optional logger for event reporting.
     */
    explicit EnigmaMachine(std::string_view fileName, std::string_view assetPath = "", ILogger* logger = nullptr);

    // Static Factory Methods

    /**
     * @brief Creates an EnigmaMachine from an asset provider and config file.
     * @param provider The asset provider for loading config files.
     * @param fileName Path to the TOML configuration file.
     * @param assetPath Optional base directory for assets.
     * @param logger Optional logger for event reporting.
     * @return enigma::Result<EnigmaMachine> on success, or error on failure.
     */
    static enigma::Result<EnigmaMachine> create(const IAssetProvider& provider, std::string_view fileName,
                                                std::string_view assetPath = "", ILogger* logger = nullptr);

    /**
     * @brief Creates an EnigmaMachine from a config file path (uses FileAssetProvider).
     * @param fileName Path to the TOML configuration file.
     * @param assetPath Optional base directory for assets.
     * @param logger Optional logger for event reporting.
     * @return enigma::Result<EnigmaMachine> on success, or error on failure.
     */
    static enigma::Result<EnigmaMachine> create(std::string_view fileName, std::string_view assetPath = "",
                                                ILogger* logger = nullptr);

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
     * @return AlphabetIndex The transformed output key.
     */
    AlphabetIndex keyTransform(AlphabetIndex input);

    /**
     * @brief Processes a buffer of alphabet indexes in-place.
     * @param buffer The span of characters to transform.
     */
    void processBuffer(std::span<AlphabetIndex> buffer);

    /**
     * @brief Sets the logger for the machine.
     * @param logger The logger to use.
     */
    void setLogger(ILogger* logger);

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
    void onRotorStepped(int rotorIndex, AlphabetIndex position) override;
    void onCharEncrypted(char input, char output) override;
};

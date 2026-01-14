#pragma once

#include <array>
#include <string>
#include <string_view>
#include <vector>
#include "PlugBoard.hpp"  // For Pair_t and PLUGBOARD_MAX_PAIRS
#include "config.hpp"     // For TRANSFORMER_SIZE

/**
 * @brief Configuration structure for an individual Rotor.
 * Contains the specific wiring and notch information required to construct a Rotor.
 */
struct RotorConfig {
    /** @brief The position (0 - (TRANSFORMER_SIZE - 1) ) at which the rotor triggers the turnover of the next rotor. */
    int notchPosition = 0;
    /** @brief The internal wiring permutation array (forward direction). Must be of size TRANSFORMER_SIZE. */
    std::vector<int> wiring;
};

/**
 * @brief Configuration structure for a Reflector.
 * Contains the specific wiring map required to construct a Reflector.
 */
struct ReflectorConfig {
    /** @brief The internal wiring permutation array (reflection map). Must be of size TRANSFORMER_SIZE. */
    std::vector<int> wiring;
};

/**
 * @brief Configuration class for the Enigma Machine.
 * Encapsulates all the parameters required to initialize the Enigma Machine, including
 * rotor selection, initial positions, wiring configurations, and plugboard settings.
 *
 * This class serves as a central point for loading and validating configuration data
 * from files (TOML) or manual setup before passing it to the EnigmaMachine logic.
 */
class EnigmaMachineConfig {
private:
    int rotorCount = 0;
    std::vector<int> rotorPositions;
    std::vector<RotorConfig> rotors;
    ReflectorConfig reflector;
    std::array<Pair_t, PLUGBOARD_MAX_PAIRS> plugBoardPairs;

public:
    /**
     * @brief Default constructor.
     * Initializes the configuration with default values.
     * Plugboard pairs are initialized to -1 (unused).
     */
    EnigmaMachineConfig();

    /**
     * @brief Loads the complete Enigma Machine configuration from a TOML file.
     *
     * Parses the specified TOML file to extract rotor count, positions, file paths for rotors/reflector,
     * and plugboard settings. It then recursively loads the individual rotor/reflector configurations.
     *
     * @param fileName The path to the main configuration TOML file.
     * @param assetPath An optional base directory path to prepend to relative paths found in the config file.
     * @return EnigmaMachineConfig A fully populated configuration object.
     * @throws std::runtime_error If parsing fails, file not found, or validation checks (e.g., size mismatches) fail.
     */
    static EnigmaMachineConfig load(std::string_view fileName, std::string_view assetPath = "");

    /**
     * @brief Loads a single rotor configuration from a TOML file.
     *
     * @param fileName The path to the rotor configuration file.
     * @return RotorConfig The parsed rotor configuration containing notch and wiring.
     * @throws std::runtime_error If the file format is invalid or wiring size is incorrect.
     */
    static RotorConfig loadRotor(std::string_view fileName);

    /**
     * @brief Loads a single reflector configuration from a TOML file.
     *
     * @param fileName The path to the reflector configuration file.
     * @return ReflectorConfig The parsed reflector configuration containing the wiring map.
     * @throws std::runtime_error If the file format is invalid or wiring size is incorrect.
     */
    static ReflectorConfig loadReflector(std::string_view fileName);

    // Getters

    /**
     * @brief Gets the number of rotors configured.
     * @return int The number of rotors.
     */
    [[nodiscard]] int getRotorCount() const { return rotorCount; }

    /**
     * @brief Gets the initial positions of the rotors.
     * @return const std::vector<int>& Reference to the vector of initial positions.
     */
    [[nodiscard]] const std::vector<int>& getRotorPositions() const { return rotorPositions; }

    /**
     * @brief Gets the configurations for all rotors.
     * @return const std::vector<RotorConfig>& Reference to the vector of rotor configurations.
     */
    [[nodiscard]] const std::vector<RotorConfig>& getRotors() const { return rotors; }

    /**
     * @brief Gets the configuration for the reflector.
     * @return const ReflectorConfig& Reference to the reflector configuration.
     */
    [[nodiscard]] const ReflectorConfig& getReflector() const { return reflector; }

    /**
     * @brief Gets the plugboard configuration pairs.
     * @return const std::array<Pair_t, PLUGBOARD_MAX_PAIRS>& Reference to the array of plugboard pairs.
     */
    [[nodiscard]] const std::array<Pair_t, PLUGBOARD_MAX_PAIRS>& getPlugBoardPairs() const { return plugBoardPairs; }
};

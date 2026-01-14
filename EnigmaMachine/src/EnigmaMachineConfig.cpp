/**
 * @file EnigmaMachineConfig.cpp
 * @brief Implementation of the EnigmaMachineConfig class.
 */

#include "EnigmaMachineConfig.hpp"
#include <stdexcept>
#include <toml.hpp>
#include "config.hpp"

namespace {

/**
 * @brief Validates the common fields of a transformer (Rotor or Reflector) configuration.
 * @internal
 *
 * Checks if the 'size' field matches TRANSFORMER_SIZE and if the 'type' field matches
 * the expected string ("rotor" or "reflector").
 *
 * @param data The parsed TOML data structure.
 * @param expectedType The expected type string (e.g., "rotor").
 * @param fileName The name of the file being parsed (for error reporting).
 * @throws std::runtime_error If validation fails or fields are missing.
 */
void validateTransformerConfig(const toml::value& data, const std::string& expectedType, const std::string& fileName) {
    try {
        auto size = toml::find<int>(data, "size");
        if (size != TRANSFORMER_SIZE) {
            throw std::runtime_error("Transformer size mismatch: expected " + std::to_string(TRANSFORMER_SIZE) +
                                     ", got " + std::to_string(size));
        }

        auto typeStr = toml::find<std::string>(data, "type");
        if (typeStr != expectedType) {
            throw std::runtime_error("Wrong config file: expected " + expectedType + ", got " + typeStr);
        }
    } catch (const std::exception& e) {
        throw std::runtime_error("Validation error in " + fileName + ": " + e.what());
    }
}
}  // namespace

/**
 * @details
 * Initializes the `plugBoardPairs` array with -1 to indicate that no connections
 * are set by default.
 */
EnigmaMachineConfig::EnigmaMachineConfig() {
    for (auto& pair : plugBoardPairs) {
        pair.a = -1;
        pair.b = -1;
    }
}

/**
 * @details
 * Uses `toml::parse` to read the file. It delegates to `validateTransformerConfig`
 * to ensure the file is a valid rotor configuration.
 *
 * The `wiring` vector is explicitly checked to ensure it has the correct size
 * defined by `TRANSFORMER_SIZE`.
 */
RotorConfig EnigmaMachineConfig::loadRotor(std::string_view fileName) {
    auto rotorData = toml::parse(std::string(fileName));
    validateTransformerConfig(rotorData, "rotor", std::string(fileName));

    RotorConfig rotorConfig;
    rotorConfig.notchPosition = toml::find<int>(rotorData, "rotor", "notchPosition");
    rotorConfig.wiring = toml::find<std::vector<int>>(rotorData, "rotor", "forward");

    if (rotorConfig.wiring.size() != TRANSFORMER_SIZE) {
        throw std::runtime_error("Error: Rotor wiring size mismatch in " + std::string(fileName));
    }
    return rotorConfig;
}

/**
 * @details
 * Similar to `loadRotor`, this function uses `toml::parse` and `validateTransformerConfig`.
 * It extracts the reflection map and ensures it adheres to the `TRANSFORMER_SIZE`.
 */
ReflectorConfig EnigmaMachineConfig::loadReflector(std::string_view fileName) {
    auto reflectorData = toml::parse(std::string(fileName));
    validateTransformerConfig(reflectorData, "reflector", std::string(fileName));

    ReflectorConfig reflectorConfig;
    reflectorConfig.wiring = toml::find<std::vector<int>>(reflectorData, "reflector", "map");
    if (reflectorConfig.wiring.size() != TRANSFORMER_SIZE) {
        throw std::runtime_error("Error: Reflector wiring size mismatch in " + std::string(fileName));
    }
    return reflectorConfig;
}

/**
 * @details
 * This function orchestrates the loading of the entire machine configuration.
 *
 * Implementation Steps:
 * 1. Parses the main TOML file.
 * 2. Validates consistency between `RotorCount`, `RotorPositions`, and `RotorFiles`.
 * 3. Resolves relative paths using the optional `assetPath`.
 * 4. Iteratively loads each rotor using `loadRotor`.
 * 5. Loads the reflector using `loadReflector`.
 * 6. Parses and populates the plugboard pairs, ensuring the count does not exceed `PLUGBOARD_MAX_PAIRS`.
 */
EnigmaMachineConfig EnigmaMachineConfig::load(std::string_view fileName, std::string_view assetPath) {
    EnigmaMachineConfig config;
    auto data = toml::parse(std::string(fileName));

    config.rotorCount = toml::find<int>(data, "rotors", "RotorCount");
    config.rotorPositions = toml::find<std::vector<int>>(data, "rotors", "RotorPositions");
    auto rotorFilePaths = toml::find<std::vector<std::string>>(data, "rotors", "RotorFiles");

    if (static_cast<size_t>(config.rotorCount) != config.rotorPositions.size() ||
        static_cast<size_t>(config.rotorCount) != rotorFilePaths.size()) {
        throw std::runtime_error("Error: Number of rotors, positions, and files do not match.");
    }

    std::string prefix = std::string(assetPath);
    if (!prefix.empty() && prefix.back() != '/') {
        prefix += "/";
    }

    for (const auto& rotorFile : rotorFilePaths) {
        config.rotors.push_back(loadRotor(prefix + rotorFile));
    }

    auto reflectorFile = toml::find<std::string>(data, "ReflectorFile");
    config.reflector = loadReflector(prefix + reflectorFile);

    auto plugsCount = toml::find<int>(data, "plugboard", "PlugCount");
    if (plugsCount > PLUGBOARD_MAX_PAIRS) {
        throw std::runtime_error("Error: Plugboard pairs exceed maximum allowed.");
    }

    auto plugBoardArr = toml::find<std::vector<toml::value>>(data, "plugboard", "PlugBoardPairs");
    if (plugBoardArr.size() != static_cast<size_t>(plugsCount)) {
        throw std::runtime_error("Error: Plugboard pairs count does not match specified count.");
    }

    for (int i = 0; i < plugsCount; i++) {
        config.plugBoardPairs.at(i).a = toml::find<int>(plugBoardArr.at(i), "from");
        config.plugBoardPairs.at(i).b = toml::find<int>(plugBoardArr.at(i), "to");
    }

    return config;
}
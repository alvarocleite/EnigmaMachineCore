#include "EnigmaConfigLoader.hpp"
#include <sstream>
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

RotorConfig EnigmaConfigLoader::loadRotor(const IAssetProvider& provider, const FileName& fileName) {
    std::istringstream stream(provider.loadAsset(fileName.string()));
    // Parse from stream, pass filename for error reporting
    auto rotorData = toml::parse(stream, fileName.string());
    validateTransformerConfig(rotorData, "rotor", fileName.string());

    RotorConfig rotorConfig;
    rotorConfig.notchPosition = toml::find<int>(rotorData, "rotor", "notchPosition");
    rotorConfig.wiring = toml::find<std::vector<int>>(rotorData, "rotor", "forward");

    if (rotorConfig.wiring.size() != TRANSFORMER_SIZE) {
        throw std::runtime_error("Error: Rotor wiring size mismatch in " + fileName.string());
    }
    return rotorConfig;
}

ReflectorConfig EnigmaConfigLoader::loadReflector(const IAssetProvider& provider, const FileName& fileName) {
    std::istringstream stream(provider.loadAsset(fileName.string()));
    auto reflectorData = toml::parse(stream, fileName.string());
    validateTransformerConfig(reflectorData, "reflector", fileName.string());

    ReflectorConfig reflectorConfig;
    reflectorConfig.wiring = toml::find<std::vector<int>>(reflectorData, "reflector", "map");
    if (reflectorConfig.wiring.size() != TRANSFORMER_SIZE) {
        throw std::runtime_error("Error: Reflector wiring size mismatch in " + fileName.string());
    }
    return reflectorConfig;
}

EnigmaMachineConfig EnigmaConfigLoader::load(const IAssetProvider& provider, const FileName& fileName,
                                             const AssetPath& assetPath) {
    std::istringstream stream(provider.loadAsset(fileName.string()));
    auto data = toml::parse(stream, fileName.string());

    int rotorCount = toml::find<int>(data, "rotors", "RotorCount");
    std::vector<int> rotorPositions = toml::find<std::vector<int>>(data, "rotors", "RotorPositions");
    auto rotorFilePaths = toml::find<std::vector<std::string>>(data, "rotors", "RotorFiles");

    if (static_cast<size_t>(rotorCount) != rotorPositions.size() ||
        static_cast<size_t>(rotorCount) != rotorFilePaths.size()) {
        throw std::runtime_error("Error: Number of rotors, positions, and files do not match.");
    }

    std::vector<RotorConfig> rotors;
    for (const auto& rotorFile : rotorFilePaths) {
        rotors.push_back(loadRotor(provider, FileName(assetPath / rotorFile)));
    }

    auto reflectorFile = toml::find<std::string>(data, "ReflectorFile");
    auto reflector = loadReflector(provider, FileName(assetPath / reflectorFile));
    auto plugsCount = toml::find<int>(data, "plugboard", "PlugCount");
    if (plugsCount > PLUGBOARD_MAX_PAIRS) {
        throw std::runtime_error("Error: Plugboard pairs exceed maximum allowed.");
    }

    auto plugBoardArr = toml::find<std::vector<toml::value>>(data, "plugboard", "PlugBoardPairs");
    if (plugBoardArr.size() != static_cast<size_t>(plugsCount)) {
        throw std::runtime_error("Error: Plugboard pairs count does not match specified count.");
    }

    std::array<PlugBoardPair, PLUGBOARD_MAX_PAIRS> plugBoardPairs;

    for (int i = 0; i < plugsCount; i++) {
        plugBoardPairs.at(i).sourcePortIndex = toml::find<int>(plugBoardArr.at(i), "from");
        plugBoardPairs.at(i).destinationPortIndex = toml::find<int>(plugBoardArr.at(i), "to");
    }

    EnigmaMachineConfig newConfig;
    newConfig.rotorCount = rotorCount;
    newConfig.rotorPositions = rotorPositions;
    newConfig.rotors = rotors;
    newConfig.reflector = reflector;
    newConfig.plugBoardPairs = plugBoardPairs;

    return newConfig;
}

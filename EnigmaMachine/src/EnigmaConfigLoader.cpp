#include "EnigmaConfigLoader.hpp"
#include <sstream>
#include <stdexcept>
#include <toml.hpp>

#include "EnigmaConfig.hpp"
#include "EnigmaError.hpp"

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
 * @return enigma::EnigmaError The error code, or None if validation passes.
 */
enigma::EnigmaError validateTransformerConfig(const toml::value& data, const std::string& expectedType) {
    try {
        auto size = toml::find<int>(data, "size");
        if (size != enigma::TRANSFORMER_SIZE) {
            return enigma::EnigmaError::TransformerSizeMismatch;
        }

        auto typeStr = toml::find<std::string>(data, "type");
        if (typeStr != expectedType) {
            return enigma::EnigmaError::ConfigCountMismatch;
        }
    } catch (const std::out_of_range&) {
        return enigma::EnigmaError::ConfigFieldMissing;
    } catch (const std::exception&) {
        return enigma::EnigmaError::ConfigCountMismatch;
    }
    return enigma::EnigmaError::None;
}
}  // namespace

enigma::Result<RotorConfig> EnigmaConfigLoader::loadRotor(const IAssetProvider& provider, const FileName& fileName) {
    auto assetResult = provider.loadAsset(fileName.string());
    if (!assetResult) {
        return nonstd::make_unexpected(assetResult.error());
    }

    std::istringstream stream(*assetResult);
    auto rotorData = toml::parse(stream, fileName.string());

    auto error = validateTransformerConfig(rotorData, "rotor");
    if (error != enigma::EnigmaError::None) {
        return nonstd::make_unexpected(error);
    }

    RotorConfig rotorConfig;
    rotorConfig.notchPosition = toml::find<AlphabetIndex>(rotorData, "rotor", "notchPosition");
    rotorConfig.wiring = toml::find<std::array<AlphabetIndex, enigma::TRANSFORMER_SIZE>>(rotorData, "rotor", "forward");

    return rotorConfig;
}

enigma::Result<ReflectorConfig> EnigmaConfigLoader::loadReflector(const IAssetProvider& provider,
                                                                  const FileName& fileName) {
    auto assetResult = provider.loadAsset(fileName.string());
    if (!assetResult) {
        return nonstd::make_unexpected(assetResult.error());
    }

    std::istringstream stream(*assetResult);
    auto reflectorData = toml::parse(stream, fileName.string());

    auto error = validateTransformerConfig(reflectorData, "reflector");
    if (error != enigma::EnigmaError::None) {
        return nonstd::make_unexpected(error);
    }

    ReflectorConfig reflectorConfig;
    reflectorConfig.wiring =
        toml::find<std::array<AlphabetIndex, enigma::TRANSFORMER_SIZE>>(reflectorData, "reflector", "map");
    return reflectorConfig;
}

enigma::Result<EnigmaMachineConfig> EnigmaConfigLoader::load(const IAssetProvider& provider, const FileName& fileName,
                                                             const AssetPath& assetPath) {
    auto assetResult = provider.loadAsset(fileName.string());
    if (!assetResult) {
        return nonstd::make_unexpected(assetResult.error());
    }

    std::istringstream stream(*assetResult);
    auto data = toml::parse(stream, fileName.string());

    int rotorCount = toml::find<int>(data, "rotors", "RotorCount");
    auto rotorPositions = toml::find<std::vector<AlphabetIndex>>(data, "rotors", "RotorPositions");
    auto rotorFilePaths = toml::find<std::vector<std::string>>(data, "rotors", "RotorFiles");

    if (static_cast<size_t>(rotorCount) != rotorPositions.size() ||
        static_cast<size_t>(rotorCount) != rotorFilePaths.size()) {
        return nonstd::make_unexpected(enigma::EnigmaError::ConfigCountMismatch);
    }

    std::vector<RotorConfig> rotors;
    rotors.reserve(rotorFilePaths.size());
    for (const auto& rotorFile : rotorFilePaths) {
        auto rotorResult = loadRotor(provider, FileName(assetPath / rotorFile));
        if (!rotorResult) {
            return nonstd::make_unexpected(rotorResult.error());
        }
        rotors.push_back(*rotorResult);
    }

    auto reflectorFile = toml::find<std::string>(data, "ReflectorFile");
    auto reflectorResult = loadReflector(provider, FileName(assetPath / reflectorFile));
    if (!reflectorResult) {
        return nonstd::make_unexpected(reflectorResult.error());
    }

    auto plugsCount = toml::find<int>(data, "plugboard", "PlugCount");
    if (plugsCount > enigma::MAX_PLUGBOARD_PAIRS) {
        return nonstd::make_unexpected(enigma::EnigmaError::PlugBoardExceedsMaximum);
    }

    auto plugBoardArr = toml::find<std::vector<toml::value>>(data, "plugboard", "PlugBoardPairs");
    if (plugBoardArr.size() != static_cast<size_t>(plugsCount)) {
        return nonstd::make_unexpected(enigma::EnigmaError::PlugBoardCountMismatch);
    }

    std::array<PlugBoardPair, enigma::MAX_PLUGBOARD_PAIRS> plugBoardPairs;

    for (int i = 0; i < plugsCount; i++) {
        plugBoardPairs.at(i).sourcePortIndex = toml::find<int>(plugBoardArr.at(i), "from");
        plugBoardPairs.at(i).destinationPortIndex = toml::find<int>(plugBoardArr.at(i), "to");
    }

    EnigmaMachineConfig newConfig;
    newConfig.rotorCount = rotorCount;
    newConfig.rotorPositions = std::move(rotorPositions);
    newConfig.rotors = std::move(rotors);
    newConfig.reflector = *reflectorResult;
    newConfig.plugBoardPairs = plugBoardPairs;

    return newConfig;
}

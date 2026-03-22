/**
 * @file
 * @brief Implementation of the EnigmaMachine class.
 */

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "EnigmaConfig.hpp"
#include "EnigmaConfigLoader.hpp"
#include "EnigmaData.hpp"
#include "EnigmaMachine.hpp"
#include "EnigmaMachineConfig.hpp"
#include "FileAssetProvider.hpp"
#include "PlugBoard.hpp"
#include "RotorBox.hpp"

namespace fs = std::filesystem;

/**
 * @brief Resolves the default asset path based on execution context.
 * Checks for a local 'assets/' folder first, otherwise falls back to the
 * path defined during installation.
 */
static std::string resolveDefaultAssetPath() {
    // Check for local 'assets' directory
    if (fs::exists("assets") && fs::is_directory("assets")) {
        return "assets/";
    }

#ifdef ENIGMA_INSTALL_ASSETS_PATH
    // Fallback to the installed assets path if defined via CMake
    if (fs::exists(ENIGMA_INSTALL_ASSETS_PATH) && fs::is_directory(ENIGMA_INSTALL_ASSETS_PATH)) {
        return ENIGMA_INSTALL_ASSETS_PATH;
    }
#endif

    // Default to the header-defined constant if everything else fails
    return std::string(enigma::assetsDir);
}

/**
 * @details Loads default configuration files (Rotor1, Rotor2, Rotor3, Reflector)
 * from the global assets directory defined in `config.hpp`.
 */
using FileName = EnigmaConfigLoader::FileName;
using AssetPath = EnigmaConfigLoader::AssetPath;
EnigmaMachine::EnigmaMachine(ILogger* logger) : logger(logger) {
    FileAssetProvider provider;
    fs::path assetsDirectory(resolveDefaultAssetPath());
    try {
        std::vector<RotorConfig> rotors;
        rotors.push_back(
            EnigmaConfigLoader::loadRotor(provider, FileName(assetsDirectory / enigma::defaultRotor1File)));
        rotors.push_back(
            EnigmaConfigLoader::loadRotor(provider, FileName(assetsDirectory / enigma::defaultRotor2File)));
        rotors.push_back(
            EnigmaConfigLoader::loadRotor(provider, FileName(assetsDirectory / enigma::defaultRotor3File)));
        auto reflector =
            EnigmaConfigLoader::loadReflector(provider, FileName(assetsDirectory / enigma::defaultReflectorFile));

        rotorBox = std::make_unique<RotorBox>(std::vector<AlphabetIndex>{0, 0, 0}, rotors, reflector, logger);
        plugBoard = std::make_unique<PlugBoard>();
        rotorBox->registerObserver(this);
    } catch (const std::exception& e) {
        if (this->logger) {
            this->logger->log(LogLevel::Error, "Failed to initialize default EnigmaMachine: " + std::string(e.what()));
        }
        throw;
    }
}

EnigmaMachine::EnigmaMachine(const IAssetProvider& provider, std::string_view fileName, std::string_view assetPath,
                             ILogger* logger)
    : EnigmaMachine(EnigmaConfigLoader::load(provider, FileName(fileName), AssetPath(assetPath)), logger) {}

EnigmaMachine::EnigmaMachine(const EnigmaMachineConfig& config, ILogger* logger)
    : rotorBox(std::make_unique<RotorBox>(config.rotorPositions, config.rotors, config.reflector, logger)),
      plugBoard(std::make_unique<PlugBoard>(config.plugBoardPairs)),
      logger(logger) {
    rotorBox->registerObserver(this);
}

EnigmaMachine::EnigmaMachine(EnigmaMachineConfig&& config, ILogger* logger)
    : rotorBox(std::make_unique<RotorBox>(std::move(config.rotorPositions), std::move(config.rotors),
                                          std::move(config.reflector), logger)),
      plugBoard(std::make_unique<PlugBoard>(config.plugBoardPairs)),
      logger(logger) {
    rotorBox->registerObserver(this);
}

EnigmaMachine::EnigmaMachine(std::string_view fileName, std::string_view assetPath, ILogger* logger)
    : EnigmaMachine(FileAssetProvider{}, fileName, assetPath, logger) {}

EnigmaMachine::EnigmaMachine(const enigma::EnigmaMachineData& data, ILogger* logger) : logger(logger) {
    std::vector<AlphabetIndex> positions;
    std::vector<RotorConfig> rotorConfigs;
    std::array<PlugBoardPair, enigma::MAX_PLUGBOARD_PAIRS> plugPairs;

    for (int i = 0; i < data.rotorCount && i < enigma::MAX_ROTORS; ++i) {
        positions.push_back(static_cast<AlphabetIndex>(data.rotorPositions[i]));

        RotorConfig rc;
        rc.notchPosition = static_cast<AlphabetIndex>(data.rotors[i].notchPosition);
        for (int j = 0; j < enigma::TRANSFORMER_SIZE; ++j) {
            rc.wiring[j] = static_cast<AlphabetIndex>(data.rotors[i].wiring[j]);
        }
        rotorConfigs.push_back(std::move(rc));
    }

    ReflectorConfig refConfig;
    for (int i = 0; i < enigma::TRANSFORMER_SIZE; ++i) {
        refConfig.wiring[i] = static_cast<AlphabetIndex>(data.reflector.wiring[i]);
    }

    for (int i = 0; i < enigma::MAX_PLUGBOARD_PAIRS; ++i) {
        plugPairs[i].sourcePortIndex = data.plugBoard.pairs[i].sourcePortIndex;
        plugPairs[i].destinationPortIndex = data.plugBoard.pairs[i].destinationPortIndex;
    }

    rotorBox = std::make_unique<RotorBox>(positions, rotorConfigs, refConfig, logger);
    plugBoard = std::make_unique<PlugBoard>(plugPairs);
    rotorBox->registerObserver(this);
}

EnigmaMachine::~EnigmaMachine() = default;

EnigmaMachine::EnigmaMachine(EnigmaMachine&& other) noexcept
    : rotorBox(std::move(other.rotorBox)),
      plugBoard(std::move(other.plugBoard)),
      observers(std::move(other.observers)),
      logger(other.logger) {
    if (rotorBox) {
        rotorBox->removeObserver(&other);
        rotorBox->registerObserver(this);
    }
}

EnigmaMachine& EnigmaMachine::operator=(EnigmaMachine&& other) noexcept {
    if (this != &other) {
        rotorBox = std::move(other.rotorBox);
        plugBoard = std::move(other.plugBoard);
        observers = std::move(other.observers);
        logger = other.logger;

        if (rotorBox) {
            rotorBox->removeObserver(&other);
            rotorBox->registerObserver(this);
        }
    }
    return *this;
}

/**
 * @details The transformation follows the historic Enigma signal path:
 * 1. Pass through Plugboard (Forward).
 * 2. Pass through RotorBox (Rotors -> Reflector -> Rotors).
 * 3. Pass through Plugboard (Reverse).
 *
 * @internal The plugboard is its own inverse, so the same swap() method is used for both entry and exit.
 * The mechanical stepping happens inside rotorBox->keyTransform() before the signal starts.
 */
AlphabetIndex EnigmaMachine::keyTransform(AlphabetIndex input) {
    AlphabetIndex originalInput = input;
    input = plugBoard->swap(input);
    input = rotorBox->keyTransform(input);
    AlphabetIndex output = plugBoard->swap(input);

    this->onCharEncrypted(static_cast<char>('A' + originalInput), static_cast<char>('A' + output));

    return output;
}

void EnigmaMachine::processBuffer(std::span<AlphabetIndex> buffer) {
    std::ranges::for_each(buffer, [this](auto& item) { item = keyTransform(item); });
}

void EnigmaMachine::setLogger(ILogger* log) {
    this->logger = log;
    if (rotorBox) {
        rotorBox->setLogger(log);
    }
}

void EnigmaMachine::registerObserver(IEnigmaObserver* observer) { observers.push_back(observer); }

void EnigmaMachine::removeObserver(IEnigmaObserver* observer) {
    auto iterator = std::ranges::remove(observers, observer).begin();
    if (iterator != observers.end()) {
        observers.erase(iterator, observers.end());
    }
}

void EnigmaMachine::onRotorStepped(int rotorIndex, AlphabetIndex position) {
    for (auto* obs : observers) {
        obs->onRotorStepped(rotorIndex, position);
    }
}

void EnigmaMachine::onCharEncrypted(char input, char output) {
    for (auto* obs : observers) {
        obs->onCharEncrypted(input, output);
    }
}

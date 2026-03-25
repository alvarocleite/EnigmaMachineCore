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
#include "EnigmaError.hpp"
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
    if (fs::exists("assets") && fs::is_directory("assets")) {
        return "assets/";
    }

#ifdef ENIGMA_INSTALL_ASSETS_PATH
    if (fs::exists(ENIGMA_INSTALL_ASSETS_PATH) && fs::is_directory(ENIGMA_INSTALL_ASSETS_PATH)) {
        return ENIGMA_INSTALL_ASSETS_PATH;
    }
#endif

    return std::string(enigma::assetsDir);
}

static void logEnigmaError(ILogger* logger, const std::string& context, enigma::EnigmaError error) {
    if (!logger) return;
    std::string msg = context + " failed with error code: " + std::to_string(static_cast<int>(error));
    logger->log(LogLevel::Error, msg);
}

using FileName = EnigmaConfigLoader::FileName;
using AssetPath = EnigmaConfigLoader::AssetPath;
EnigmaMachine::EnigmaMachine(ILogger* logger) : logger(logger) {
    FileAssetProvider provider;
    fs::path assetsDirectory(resolveDefaultAssetPath());

    std::vector<RotorConfig> rotors;
    auto rotor1Result = EnigmaConfigLoader::loadRotor(provider, FileName(assetsDirectory / enigma::defaultRotor1File));
    if (!rotor1Result) {
        logEnigmaError(logger, "Failed to load Rotor 1", rotor1Result.error());
        throw std::runtime_error("Failed to load Rotor 1");
    }
    rotors.push_back(*rotor1Result);

    auto rotor2Result = EnigmaConfigLoader::loadRotor(provider, FileName(assetsDirectory / enigma::defaultRotor2File));
    if (!rotor2Result) {
        logEnigmaError(logger, "Failed to load Rotor 2", rotor2Result.error());
        throw std::runtime_error("Failed to load Rotor 2");
    }
    rotors.push_back(*rotor2Result);

    auto rotor3Result = EnigmaConfigLoader::loadRotor(provider, FileName(assetsDirectory / enigma::defaultRotor3File));
    if (!rotor3Result) {
        logEnigmaError(logger, "Failed to load Rotor 3", rotor3Result.error());
        throw std::runtime_error("Failed to load Rotor 3");
    }
    rotors.push_back(*rotor3Result);

    auto reflectorResult =
        EnigmaConfigLoader::loadReflector(provider, FileName(assetsDirectory / enigma::defaultReflectorFile));
    if (!reflectorResult) {
        logEnigmaError(logger, "Failed to load Reflector", reflectorResult.error());
        throw std::runtime_error("Failed to load Reflector");
    }

    rotorBox = std::make_unique<RotorBox>(std::vector<AlphabetIndex>{0, 0, 0}, rotors, *reflectorResult, logger);
    plugBoard = std::make_unique<PlugBoard>();
    rotorBox->registerObserver(this);
}

EnigmaMachine::EnigmaMachine(const IAssetProvider& provider, std::string_view fileName, std::string_view assetPath,
                             ILogger* logger)
    : logger(logger) {
    auto configResult = EnigmaConfigLoader::load(provider, FileName(fileName), AssetPath(assetPath));
    if (!configResult) {
        logEnigmaError(logger, "Failed to load Enigma configuration", configResult.error());
        throw std::runtime_error("Failed to load Enigma configuration");
    }
    rotorBox =
        std::make_unique<RotorBox>(configResult->rotorPositions, configResult->rotors, configResult->reflector, logger);
    plugBoard = std::make_unique<PlugBoard>(configResult->plugBoardPairs);
    rotorBox->registerObserver(this);
}

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

enigma::Result<EnigmaMachine> EnigmaMachine::create(const IAssetProvider& provider, std::string_view fileName,
                                                    std::string_view assetPath, ILogger* logger) {
    auto configResult = EnigmaConfigLoader::load(provider, FileName(fileName), AssetPath(assetPath));
    if (!configResult) {
        logEnigmaError(logger, "Failed to load Enigma configuration", configResult.error());
        return nonstd::make_unexpected(configResult.error());
    }

    EnigmaMachine machine;
    machine.logger = logger;
    machine.rotorBox =
        std::make_unique<RotorBox>(configResult->rotorPositions, configResult->rotors, configResult->reflector, logger);
    machine.plugBoard = std::make_unique<PlugBoard>(configResult->plugBoardPairs);
    machine.rotorBox->registerObserver(&machine);
    return machine;
}

enigma::Result<EnigmaMachine> EnigmaMachine::create(std::string_view fileName, std::string_view assetPath,
                                                    ILogger* logger) {
    return create(FileAssetProvider{}, fileName, assetPath, logger);
}

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

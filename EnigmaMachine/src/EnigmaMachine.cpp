/**
 * @file
 * @brief Implementation of the EnigmaMachine class.
 */

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "EnigmaConfigLoader.hpp"
#include "EnigmaMachine.hpp"
#include "FileAssetProvider.hpp"
#include "PlugBoard.hpp"
#include "RotorBox.hpp"
#include "config.hpp"

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
    return std::string(assetsDir);
}

/**
 * @details Loads default configuration files (Rotor1, Rotor2, Rotor3, Reflector)
 * from the global assets directory defined in `config.hpp`.
 */
using FileName = EnigmaConfigLoader::FileName;
using AssetPath = EnigmaConfigLoader::AssetPath;
EnigmaMachine::EnigmaMachine() {
    FileAssetProvider provider;
    fs::path assetsDirectory(resolveDefaultAssetPath());
    try {
        std::vector<RotorConfig> rotors;
        rotors.push_back(EnigmaConfigLoader::loadRotor(provider, FileName(assetsDirectory / defaultRotor1File)));
        rotors.push_back(EnigmaConfigLoader::loadRotor(provider, FileName(assetsDirectory / defaultRotor2File)));
        rotors.push_back(EnigmaConfigLoader::loadRotor(provider, FileName(assetsDirectory / defaultRotor3File)));
        auto reflector = EnigmaConfigLoader::loadReflector(provider, FileName(assetsDirectory / defaultReflectorFile));

        rotorBox = std::make_unique<RotorBox>(std::vector<int>{0, 0, 0}, rotors, reflector);
        plugBoard = std::make_unique<PlugBoard>();
        rotorBox->registerObserver(this);
    } catch (const std::exception& e) {
        std::cerr << "Failed to initialize default EnigmaMachine: " << e.what() << "\n";
        throw;
    }
}

EnigmaMachine::EnigmaMachine(const IAssetProvider& provider, std::string_view fileName, std::string_view assetPath)
    : EnigmaMachine(EnigmaConfigLoader::load(provider, FileName(fileName), AssetPath(assetPath))) {}

EnigmaMachine::EnigmaMachine(const EnigmaMachineConfig& config)
    : rotorBox(std::make_unique<RotorBox>(config.rotorPositions, config.rotors, config.reflector)),
      plugBoard(std::make_unique<PlugBoard>(config.plugBoardPairs)) {
    rotorBox->registerObserver(this);
}

EnigmaMachine::EnigmaMachine(std::string_view fileName, std::string_view assetPath)
    : EnigmaMachine([&]() {
          FileAssetProvider provider;
          return EnigmaConfigLoader::load(provider, FileName(fileName), AssetPath(assetPath));
      }()) {}

EnigmaMachine::~EnigmaMachine() = default;

EnigmaMachine::EnigmaMachine(EnigmaMachine&& other) noexcept
    : rotorBox(std::move(other.rotorBox)),
      plugBoard(std::move(other.plugBoard)),
      observers(std::move(other.observers)) {
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
int EnigmaMachine::keyTransform(int input) {
    int originalInput = input;
    input = plugBoard->swap(input);
    input = rotorBox->keyTransform(input);
    int output = plugBoard->swap(input);

    this->onCharEncrypted(static_cast<char>('A' + originalInput), static_cast<char>('A' + output));

    return output;
}

void EnigmaMachine::registerObserver(IEnigmaObserver* observer) { observers.push_back(observer); }

void EnigmaMachine::removeObserver(IEnigmaObserver* observer) {
    auto iterator = std::remove(observers.begin(), observers.end(), observer);
    if (iterator != observers.end()) {
        observers.erase(iterator, observers.end());
    }
}

void EnigmaMachine::onRotorStepped(int rotorIndex, int position) {
    for (auto* obs : observers) {
        obs->onRotorStepped(rotorIndex, position);
    }
}

void EnigmaMachine::onCharEncrypted(char input, char output) {
    for (auto* obs : observers) {
        obs->onCharEncrypted(input, output);
    }
}
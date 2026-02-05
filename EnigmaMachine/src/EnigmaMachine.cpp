/**
 * @file
 * @brief Implementation of the EnigmaMachine class.
 */

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "EnigmaConfigLoader.hpp"
#include "EnigmaMachine.hpp"
#include "FileAssetProvider.hpp"
#include "config.hpp"

/**
 * @details Loads default configuration files (Rotor1, Rotor2, Rotor3, Reflector)
 * from the global assets directory defined in `config.hpp`.
 */
using FileName = EnigmaConfigLoader::FileName;
using AssetPath = EnigmaConfigLoader::AssetPath;
EnigmaMachine::EnigmaMachine() {
    namespace fs = std::filesystem;
    FileAssetProvider provider;
    fs::path assetsDirectory(assetsDir);
    try {
        std::vector<RotorConfig> rotors;
        rotors.push_back(EnigmaConfigLoader::loadRotor(provider, FileName(assetsDirectory / defaultRotor1File)));
        rotors.push_back(EnigmaConfigLoader::loadRotor(provider, FileName(assetsDirectory / defaultRotor2File)));
        rotors.push_back(EnigmaConfigLoader::loadRotor(provider, FileName(assetsDirectory / defaultRotor3File)));
        auto reflector = EnigmaConfigLoader::loadReflector(provider, FileName(assetsDirectory / defaultReflectorFile));
        rotorBox = RotorBox(3, {0, 0, 0}, rotors, reflector);
        rotorBox.registerObserver(this);
    } catch (const std::exception& e) {
        std::cerr << "Failed to initialize default EnigmaMachine: " << e.what() << "\n";
        throw;
    }
}

/**
 * @details Validates that the number of transformer files matches `nRotorCount + 1` (for the reflector).
 * Loads each rotor and reflector configuration from the provided file paths before initializing the RotorBox.
 */
EnigmaMachine::EnigmaMachine(int nRotorCount, const std::vector<int>& rotorPositions,
                             const std::vector<std::string>& transformerFiles) {
    if (transformerFiles.size() != static_cast<size_t>(nRotorCount) + 1) {
        throw std::invalid_argument("Error: Number of transformer files must be nRotorCount + 1 (Reflector).");
    }

    FileAssetProvider provider;
    std::vector<RotorConfig> rotors(nRotorCount);
    for (int i = 0; i < nRotorCount; ++i) {
        rotors[i] = EnigmaConfigLoader::loadRotor(provider, FileName(transformerFiles[i]));
    }
    auto reflector = EnigmaConfigLoader::loadReflector(provider, FileName(transformerFiles[nRotorCount]));

    rotorBox = RotorBox(nRotorCount, rotorPositions, rotors, reflector);
    rotorBox.registerObserver(this);
}

/**
 * @details Similar to the standard parameterized constructor, but also initializes the PlugBoard
 * with the provided pairs.
 */
EnigmaMachine::EnigmaMachine(int nRotorCount, const std::vector<int>& rotorPositions,
                             const std::vector<std::string>& transformerFiles,
                             const std::array<PlugBoardPair, PLUGBOARD_MAX_PAIRS>& plugBoardPairs)
    : plugBoard(plugBoardPairs) {
    if (transformerFiles.size() != static_cast<size_t>(nRotorCount) + 1) {
        throw std::invalid_argument("Error: Number of transformer files must be nRotorCount + 1 (Reflector).");
    }

    FileAssetProvider provider;
    std::vector<RotorConfig> rotors(nRotorCount);
    for (int i = 0; i < nRotorCount; ++i) {
        rotors[i] = EnigmaConfigLoader::loadRotor(provider, FileName(transformerFiles[i]));
    }
    auto reflector = EnigmaConfigLoader::loadReflector(provider, FileName(transformerFiles[nRotorCount]));

    rotorBox = RotorBox(nRotorCount, rotorPositions, rotors, reflector);
    rotorBox.registerObserver(this);
}

EnigmaMachine::EnigmaMachine(IAssetProvider& provider, std::string_view fileName, std::string_view assetPath)
    : EnigmaMachine(EnigmaConfigLoader::load(provider, FileName(fileName), AssetPath(assetPath))) {}

EnigmaMachine::EnigmaMachine(const EnigmaMachineConfig& config)
    : rotorBox(config.rotorCount, config.rotorPositions, config.rotors, config.reflector),
      plugBoard(config.plugBoardPairs) {
    rotorBox.registerObserver(this);
}

EnigmaMachine::EnigmaMachine(std::string_view fileName, std::string_view assetPath)
    : EnigmaMachine([&]() {
          FileAssetProvider provider;
          return EnigmaConfigLoader::load(provider, FileName(fileName), AssetPath(assetPath));
      }()) {}

EnigmaMachine::~EnigmaMachine() = default;

EnigmaMachine::EnigmaMachine(EnigmaMachine&& other) noexcept
    : rotorBox(std::move(other.rotorBox)), plugBoard(other.plugBoard), observers(std::move(other.observers)) {
    // The moved-from rotorBox (now in *this) still has 'other' as observer.
    // We must update it to point to 'this'.
    rotorBox.removeObserver(&other);
    rotorBox.registerObserver(this);
}

EnigmaMachine& EnigmaMachine::operator=(EnigmaMachine&& other) noexcept {
    if (this != &other) {
        rotorBox = std::move(other.rotorBox);
        plugBoard = other.plugBoard;
        observers = std::move(other.observers);

        // Fix observer pointer
        rotorBox.removeObserver(&other);
        rotorBox.registerObserver(this);
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
 * The mechanical stepping happens inside rotorBox.keyTransform() before the signal starts.
 */
int EnigmaMachine::keyTransform(int input) {
    int originalInput = input;
    input = plugBoard.swap(input);
    input = rotorBox.keyTransform(input);
    int output = plugBoard.swap(input);

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
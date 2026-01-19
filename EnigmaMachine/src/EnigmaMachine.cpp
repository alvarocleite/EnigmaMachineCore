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
EnigmaMachine::EnigmaMachine() {
    FileAssetProvider provider;
    try {
        std::vector<RotorConfig> rotors;
        rotors.push_back(
            EnigmaConfigLoader::loadRotor(provider, std::string(assetsDir) + std::string(defaultRotor1File)));
        rotors.push_back(
            EnigmaConfigLoader::loadRotor(provider, std::string(assetsDir) + std::string(defaultRotor2File)));
        rotors.push_back(
            EnigmaConfigLoader::loadRotor(provider, std::string(assetsDir) + std::string(defaultRotor3File)));

        ReflectorConfig reflector =
            EnigmaConfigLoader::loadReflector(provider, std::string(assetsDir) + std::string(defaultReflectorFile));

        rotorBox = RotorBox(3, {0, 0, 0}, rotors, reflector);
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
                             const std::vector<std::string>& transformerFiles)
    : plugBoard() {
    if (transformerFiles.size() != static_cast<size_t>(nRotorCount + 1)) {
        throw std::invalid_argument("Error: Number of transformer files must be nRotorCount + 1 (Reflector).");
    }

    FileAssetProvider provider;
    std::vector<RotorConfig> rotors;
    for (int i = 0; i < nRotorCount; ++i) {
        rotors.push_back(EnigmaConfigLoader::loadRotor(provider, transformerFiles[i]));
    }
    ReflectorConfig reflector = EnigmaConfigLoader::loadReflector(provider, transformerFiles[nRotorCount]);

    rotorBox = RotorBox(nRotorCount, rotorPositions, rotors, reflector);
}

/**
 * @details Similar to the standard parameterized constructor, but also initializes the PlugBoard
 * with the provided pairs.
 */
EnigmaMachine::EnigmaMachine(int nRotorCount, const std::vector<int>& rotorPositions,
                             const std::vector<std::string>& transformerFiles,
                             const std::array<Pair_t, PLUGBOARD_MAX_PAIRS>& plugBoardPairs)
    : plugBoard(plugBoardPairs) {
    if (transformerFiles.size() != static_cast<size_t>(nRotorCount + 1)) {
        throw std::invalid_argument("Error: Number of transformer files must be nRotorCount + 1 (Reflector).");
    }

    FileAssetProvider provider;
    std::vector<RotorConfig> rotors;
    for (int i = 0; i < nRotorCount; ++i) {
        rotors.push_back(EnigmaConfigLoader::loadRotor(provider, transformerFiles[i]));
    }
    ReflectorConfig reflector = EnigmaConfigLoader::loadReflector(provider, transformerFiles[nRotorCount]);

    rotorBox = RotorBox(nRotorCount, rotorPositions, rotors, reflector);
}

EnigmaMachine::EnigmaMachine(IAssetProvider& provider, std::string_view fileName, std::string_view assetPath)
    : EnigmaMachine(EnigmaConfigLoader::load(provider, fileName, assetPath)) {}

EnigmaMachine::EnigmaMachine(const EnigmaMachineConfig& config)
    : rotorBox(config.rotorCount, config.rotorPositions, config.rotors, config.reflector),
      plugBoard(config.plugBoardPairs) {}

EnigmaMachine::EnigmaMachine(std::string_view fileName, std::string_view assetPath)
    : EnigmaMachine([&]() {
          FileAssetProvider p;
          return EnigmaConfigLoader::load(p, fileName, assetPath);
      }()) {}

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

    for (auto* obs : observers) {
        obs->onCharEncrypted(static_cast<char>('A' + originalInput), static_cast<char>('A' + output));
    }

    return output;
}

void EnigmaMachine::registerObserver(IEnigmaObserver* observer) {
    observers.push_back(observer);
    rotorBox.registerObserver(observer);
}

void EnigmaMachine::removeObserver(IEnigmaObserver* observer) {
    auto it = std::remove(observers.begin(), observers.end(), observer);
    observers.erase(it, observers.end());
    rotorBox.removeObserver(observer);
}

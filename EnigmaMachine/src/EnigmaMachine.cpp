/**
 * @file
 * @brief Implementation of the EnigmaMachine class.
 */

#include <iostream>
#include <stdexcept>
#include <vector>

#include "EnigmaMachine.hpp"
#include "config.hpp"

/**
 * @details Loads default configuration files (Rotor1, Rotor2, Rotor3, Reflector)
 * from the global assets directory defined in `config.hpp`.
 */
EnigmaMachine::EnigmaMachine() {
    try {
        std::vector<RotorConfig> rotors;
        rotors.push_back(EnigmaMachineConfig::loadRotor(std::string(assetsDir) + "Rotor1.toml"));
        rotors.push_back(EnigmaMachineConfig::loadRotor(std::string(assetsDir) + "Rotor2.toml"));
        rotors.push_back(EnigmaMachineConfig::loadRotor(std::string(assetsDir) + "Rotor3.toml"));

        ReflectorConfig reflector = EnigmaMachineConfig::loadReflector(std::string(assetsDir) + "Reflector.toml");

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

    std::vector<RotorConfig> rotors;
    for (int i = 0; i < nRotorCount; ++i) {
        rotors.push_back(EnigmaMachineConfig::loadRotor(transformerFiles[i]));
    }
    ReflectorConfig reflector = EnigmaMachineConfig::loadReflector(transformerFiles[nRotorCount]);

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

    std::vector<RotorConfig> rotors;
    for (int i = 0; i < nRotorCount; ++i) {
        rotors.push_back(EnigmaMachineConfig::loadRotor(transformerFiles[i]));
    }
    ReflectorConfig reflector = EnigmaMachineConfig::loadReflector(transformerFiles[nRotorCount]);

    rotorBox = RotorBox(nRotorCount, rotorPositions, rotors, reflector);
}

EnigmaMachine::EnigmaMachine(const EnigmaMachineConfig& config)
    : rotorBox(config.getRotorCount(), config.getRotorPositions(), config.getRotors(), config.getReflector()),
      plugBoard(config.getPlugBoardPairs()) {}

EnigmaMachine::EnigmaMachine(std::string_view fileName, std::string_view assetPath)
    : EnigmaMachine(EnigmaMachineConfig::load(fileName, assetPath)) {}

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
    input = plugBoard.swap(input);
    input = rotorBox.keyTransform(input);
    return plugBoard.swap(input);
}

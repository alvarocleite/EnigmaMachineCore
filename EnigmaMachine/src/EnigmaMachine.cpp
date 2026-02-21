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
#include "PlugBoard.hpp"
#include "RotorBox.hpp"
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

        rotorBox = std::make_unique<RotorBox>(3, std::vector<int>{0, 0, 0}, rotors, reflector);
        plugBoard = std::make_unique<PlugBoard>();
        rotorBox->registerObserver(this);
    } catch (const std::exception& e) {
        std::cerr << "Failed to initialize default EnigmaMachine: " << e.what() << "\n";
        throw;
    }
}

EnigmaMachine::EnigmaMachine(IAssetProvider& provider, std::string_view fileName, std::string_view assetPath)
    : EnigmaMachine(EnigmaConfigLoader::load(provider, FileName(fileName), AssetPath(assetPath))) {}

EnigmaMachine::EnigmaMachine(const EnigmaMachineConfig& config)
    : rotorBox(std::make_unique<RotorBox>(config.rotorCount, config.rotorPositions, config.rotors, config.reflector)),
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
#include <iostream>
#include <vector>

#include "EnigmaMachine.hpp"
#include "config.hpp"

#include <toml.hpp>

EnigmaMachine::EnigmaMachine()
    : rotorBox(3, std::vector<int>{0, 0, 0},
               std::vector<std::string>{assetsDir + "Rotor1.toml", assetsDir + "Rotor2.toml", assetsDir + "Rotor3.toml",
                                        assetsDir + "Reflector.toml"}),
      plugBoard() {}

EnigmaMachine::EnigmaMachine(int nRotorCount, const std::vector<int>& rotorPositions,
                             const std::vector<std::string>& rotorFiles)
    : rotorBox(nRotorCount, rotorPositions, rotorFiles), plugBoard() {}

EnigmaMachine::EnigmaMachine(int nRotorCount, const std::vector<int>& rotorPositions,
                             const std::vector<std::string>& rotorFiles,
                             const std::array<Pair_t, PLUGBOARD_MAX_PAIRS>& plugBoardPairs)
    : rotorBox(nRotorCount, rotorPositions, rotorFiles), plugBoard(plugBoardPairs) {}

std::tuple<int, std::vector<int>, std::vector<std::string>, std::array<Pair_t, PLUGBOARD_MAX_PAIRS>>
EnigmaMachine::parseConfig(const std::string& fileName, const std::string& assetPath) {
    auto data = toml::parse(fileName);
    int nRotorCount = toml::find<int>(data, "rotors", "RotorCount");
    auto rotorPositions = toml::find<std::vector<int>>(data, "rotors", "RotorPositions");
    auto rotorFiles = toml::find<std::vector<std::string>>(data, "rotors", "RotorFiles");
    if (static_cast<size_t>(nRotorCount) != rotorPositions.size() ||
        static_cast<size_t>(nRotorCount) != rotorFiles.size()) {
        throw std::runtime_error("Error: Number of rotors, positions, and files do not match.");
    }

    std::string prefix = assetPath;
    if (!prefix.empty() && prefix.back() != '/') {
        prefix += "/";
    }

    for (auto& rotorFile : rotorFiles) {
        rotorFile = prefix + rotorFile;  // Ensure full path is used
    }

    auto reflectorFile = prefix + toml::find<std::string>(data, "ReflectorFile");
    rotorFiles.push_back(reflectorFile);
    auto plugsCount = toml::find<int>(data, "plugboard", "PlugCount");
    if (plugsCount > PLUGBOARD_MAX_PAIRS) {
        throw std::runtime_error("Error: Plugboard pairs exceed maximum allowed.");
    }
    auto plugBoardArr = toml::find<std::vector<toml::value>>(data, "plugboard", "PlugBoardPairs");
    if (plugBoardArr.size() != static_cast<size_t>(plugsCount)) {
        throw std::runtime_error("Error: Plugboard pairs count does not match specified count.");
    }
    std::array<Pair_t, PLUGBOARD_MAX_PAIRS> plugBoardPairs;
    int i;
    for (i = 0; i < plugsCount; i++) {
        plugBoardPairs.at(i).a = toml::find<int>(plugBoardArr.at(i), "from");
        plugBoardPairs.at(i).b = toml::find<int>(plugBoardArr.at(i), "to");
    }
    while (i < PLUGBOARD_MAX_PAIRS) {
        plugBoardPairs.at(i).a = -1;  // Mark unused pairs
        plugBoardPairs.at(i).b = -1;
        i++;
    }

    return {nRotorCount, rotorPositions, rotorFiles, plugBoardPairs};
}

EnigmaMachine::EnigmaMachine(
    std::tuple<int, std::vector<int>, std::vector<std::string>, std::array<Pair_t, PLUGBOARD_MAX_PAIRS>> config)
    : EnigmaMachine(std::get<0>(config), std::get<1>(config), std::get<2>(config), std::get<3>(config)) {}

EnigmaMachine::EnigmaMachine(std::string fileName, std::string assetPath)
    : EnigmaMachine(parseConfig(fileName, assetPath)) {}

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

#include <vector>
#include <iostream>

#include "../include/EnigmaMachine.hpp"
#include "../../config/config.hpp"

#include <toml.hpp> 

const std::array<Pair_t, PLUGBOARD_MAX_PAIRS> emptyPlugboard = []{
    std::array<Pair_t, PLUGBOARD_MAX_PAIRS> arr{};
    for (auto& p : arr) { p.a = -1; p.b = -1; }
    return arr;
}();

/**
 * @brief Default constructor for the EnigmaMachine class.
 * Initializes the rotor box with 3 rotors, all starting at position 0,
 * and uses default rotor and reflector files.
 */
EnigmaMachine::EnigmaMachine()
    : rotorBox(3, std::vector<int> {0, 0, 0},
              std::vector<std::string> {assetsDir + "Rotor1.toml", 
                                        assetsDir + "Rotor2.toml", 
                                        assetsDir + "Rotor3.toml", 
                                        assetsDir + "Reflector.toml"}),
        plugBoard(emptyPlugboard)
{}

/**
 * @brief Constructor for the EnigmaMachine class.
 * Initializes the rotor box with a specified number of rotors, their initial positions,
 * and corresponding files for each rotor and reflector.
 * 
 * @param nRotorCount The number of rotors in the rotor box.
 * @param rotorPositions A vector containing the initial positions of each rotor.
 * @param rotorFiles A vector containing the file names for each rotor and reflector.
 */
EnigmaMachine::EnigmaMachine(int nRotorCount, const std::vector<int> &rotorPositions, const std::vector<std::string> &rotorFiles)
    : rotorBox(nRotorCount, rotorPositions, rotorFiles),
        plugBoard(emptyPlugboard)
{}

/**
 * @brief Constructor for the EnigmaMachine class.
 * Initializes the rotor box with a specified number of rotors, their initial positions,
 * and corresponding files for each rotor and reflector.
 * This constructor also allows for the inclusion of a plugboard with specified pairs.
 * 
 * @param nRotorCount The number of rotors in the rotor box.
 * @param rotorPositions A vector containing the initial positions of each rotor.
 * @param rotorFiles A vector containing the file names for each rotor and reflector.
 * @param plugBoardPairs An array of pairs for the plugboard, allowing for additional transformations.
 */
EnigmaMachine::EnigmaMachine(int nRotorCount, const std::vector<int> &rotorPositions, const std::vector<std::string> &rotorFiles, const std::array<Pair_t, PLUGBOARD_MAX_PAIRS> &plugBoardPairs)
    : rotorBox(nRotorCount, rotorPositions, rotorFiles), plugBoard(plugBoardPairs)
{}

/**
 * @brief Function used by fileName Constructor for the EnigmaMachine class.
 * Parses the configuration file to extract the number of rotors, their positions, files, and plugboard pairs.
 * 
 * @param fileName The name of the configuration file containing the settings for the Enigma machine.
 */
static std::tuple<int, std::vector<int>, std::vector<std::string>, std::array<Pair_t, PLUGBOARD_MAX_PAIRS>>
parseConfig(const std::string& fileName) {
    auto data = toml::parse(fileName);
    int nRotorCount = toml::find<int>(data, "rotors", "RotorCount");
    auto rotorPositions = toml::find<std::vector<int>>(data, "rotors", "RotorPositions");
    auto rotorFiles = toml::find<std::vector<std::string>>(data, "rotors", "RotorFiles");
    if (nRotorCount != rotorPositions.size() || nRotorCount != rotorFiles.size()) {
        throw std::runtime_error("Error: Number of rotors, positions, and files do not match.");
    }
    for (auto& rotorFile : rotorFiles) {
        rotorFile = assetsDir + rotorFile; // Ensure full path is used
    }

    auto reflectorFile = assetsDir + toml::find<std::string>(data, "ReflectorFile");
    rotorFiles.push_back(reflectorFile);
    auto plugsCount = toml::find<int>(data, "plugboard", "PlugCount");
    if (plugsCount > PLUGBOARD_MAX_PAIRS) {
        throw std::runtime_error("Error: Plugboard pairs exceed maximum allowed.");
    }
    auto plugBoardArr = toml::find<std::vector<toml::value>>(data, "plugboard", "PlugBoardPairs");
    if (plugBoardArr.size() != plugsCount) {
        throw std::runtime_error("Error: Plugboard pairs count does not match specified count.");
    }
    std::array<Pair_t, PLUGBOARD_MAX_PAIRS> plugBoardPairs;
    int i;
    for (i = 0; i < plugsCount; i++) {
        plugBoardPairs[i].a = toml::find<int>(plugBoardArr[i], "from");
        plugBoardPairs[i].b = toml::find<int>(plugBoardArr[i], "to");
    }
    while (i < PLUGBOARD_MAX_PAIRS) {
        plugBoardPairs[i].a = -1; // Mark unused pairs
        plugBoardPairs[i].b = -1;
        i++;
    }
    
    return {nRotorCount, rotorPositions, rotorFiles, plugBoardPairs};
}

/**
 * @brief Constructor for the EnigmaMachine class.
 * Initializes the rotor box and plugboard based on a configuration file.
 * 
 * @param fileName The name of the configuration file containing the rotor and plugboard settings.
 */
EnigmaMachine::EnigmaMachine(std::string fileName)
    : EnigmaMachine(
        std::get<0>(parseConfig(fileName)),
        std::get<1>(parseConfig(fileName)),
        std::get<2>(parseConfig(fileName)),
        std::get<3>(parseConfig(fileName))
    )
{}

EnigmaMachine::~EnigmaMachine(){}

/**
 * @brief Transforms the input key through the rotor box.
 * This function first applies the plugboard transformation to the input key,
 * then call rotorBox key transform (which updates the rotor positions, and transforms the input through the rotors), 
 * and finally applies the plugboard transformation again to the output key.
 * 
 * @param input The input key to be transformed.
 * @return int The transformed output key.
 */
int EnigmaMachine::keyTransform(int input){
    input = plugBoard.swap(input);
    input = rotorBox.keyTransform(input);
    return plugBoard.swap(input);
}

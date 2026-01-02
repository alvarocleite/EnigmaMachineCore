#include <vector>
#include <iostream>

#include "EnigmaMachine.hpp"
#include "config.hpp"

#include <toml.hpp> 


/**
 * @brief Default Constructor for the EnigmaMachine class.
 *
 * Initializes a standard Enigma Machine configuration with:
 * - 3 Rotors (Rotor1, Rotor2, Rotor3) positioned at 0.
 * - A standard Reflector.
 * - An empty PlugBoard (identity mapping).
 *
 * This configuration is useful for basic testing or default behavior.
 */
EnigmaMachine::EnigmaMachine()
    : rotorBox(3, std::vector<int> {0, 0, 0},
              std::vector<std::string> {assetsDir + "Rotor1.toml", 
                                        assetsDir + "Rotor2.toml", 
                                        assetsDir + "Rotor3.toml", 
                                        assetsDir + "Reflector.toml"}),
        plugBoard()
{}

/**
 * @brief Parameterized Constructor for the EnigmaMachine class (No PlugBoard).
 *
 * Initializes the machine with a custom RotorBox configuration and an empty PlugBoard.
 *
 * @param nRotorCount The number of rotors to be used in the RotorBox.
 * @param rotorPositions A vector defining the initial rotational position (0-25) for each rotor.
 * @param rotorFiles A vector of file paths defining the wiring for each rotor and the reflector.
 *                   The last file in the list is expected to be the Reflector.
 */
EnigmaMachine::EnigmaMachine(int nRotorCount, const std::vector<int> &rotorPositions, const std::vector<std::string> &rotorFiles)
    : rotorBox(nRotorCount, rotorPositions, rotorFiles),
        plugBoard()
{}

/**
 * @brief Parameterized Constructor for the EnigmaMachine class (With PlugBoard).
 *
 * Initializes the machine with a fully custom configuration for both the RotorBox and PlugBoard.
 *
 * @param nRotorCount The number of rotors to be used in the RotorBox.
 * @param rotorPositions A vector defining the initial rotational position (0-25) for each rotor.
 * @param rotorFiles A vector of file paths defining the wiring for each rotor and the reflector.
 *                   The last file in the list is expected to be the Reflector.
 * @param plugBoardPairs An array of `Pair_t` defining the swaps to be configured on the PlugBoard.
 */
EnigmaMachine::EnigmaMachine(int nRotorCount, const std::vector<int> &rotorPositions, const std::vector<std::string> &rotorFiles, const std::array<Pair_t, PLUGBOARD_MAX_PAIRS> &plugBoardPairs)
    : rotorBox(nRotorCount, rotorPositions, rotorFiles), plugBoard(plugBoardPairs)
{}

/**
 * @brief Function used by fileName Constructor for the EnigmaMachine class.
 * Parses the configuration file to extract the number of rotors, their positions, files, and plugboard pairs.
 * 
 * @param fileName The name of the configuration file containing the settings for the Enigma machine.
 * @return A tuple containing the number of rotors, their positions, files, and plugboard pairs.
 * @throws std::runtime_error if the configuration file is invalid or if the number of rotors, positions, and files do not match.
 */
std::tuple<int, std::vector<int>, std::vector<std::string>, std::array<Pair_t, PLUGBOARD_MAX_PAIRS>>
EnigmaMachine::parseConfig(const std::string& fileName) {
    auto data = toml::parse(fileName);
    int nRotorCount = toml::find<int>(data, "rotors", "RotorCount");
    auto rotorPositions = toml::find<std::vector<int>>(data, "rotors", "RotorPositions");
    auto rotorFiles = toml::find<std::vector<std::string>>(data, "rotors", "RotorFiles");
    if (static_cast<size_t>(nRotorCount) != rotorPositions.size() || static_cast<size_t>(nRotorCount) != rotorFiles.size()) {
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
    if (plugBoardArr.size() != static_cast<size_t>(plugsCount)) {
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
 * @brief Private Delegating Constructor.
 *
 * Used as an intermediary to unpack the configuration tuple returned by `parseConfig`
 * and pass the individual components to the main public constructor.
 *
 * @param config A tuple containing: {RotorCount, RotorPositions, RotorFiles, PlugBoardPairs}.
 */
EnigmaMachine::EnigmaMachine(std::tuple<int, std::vector<int>, std::vector<std::string>, std::array<Pair_t, PLUGBOARD_MAX_PAIRS>> config)
    : EnigmaMachine(std::get<0>(config), std::get<1>(config), std::get<2>(config), std::get<3>(config))
{}

/**
 * @brief File-based Constructor for the EnigmaMachine class.
 *
 * Initializes the machine by parsing a TOML configuration file.
 *
 * This constructor delegates the parsing logic to `parseConfig` and then delegates
 * initialization to the main parameterized constructor.
 *
 * @param fileName The path to the TOML configuration file.
 * @throws std::runtime_error If the file cannot be parsed or contains invalid configuration data.
 */
EnigmaMachine::EnigmaMachine(std::string fileName)
    : EnigmaMachine(parseConfig(fileName))
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

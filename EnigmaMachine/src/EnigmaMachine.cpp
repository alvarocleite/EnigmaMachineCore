#include <vector>

#include "../include/EnigmaMachine.hpp"
#include "../../config/config.hpp"

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
        plugBoard(std::array<Pair_t, PLUGBOARD_MAX_PAIRS> {{}})
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
        plugBoard(std::array<Pair_t, PLUGBOARD_MAX_PAIRS> {{}})
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

EnigmaMachine::~EnigmaMachine(){}

/**
 * @brief Transforms the input key through the rotor box.
 * This function updates the rotor positions, transforms the input through the rotors and reflector,
 * and returns the transformed output.
 * 
 * @param input The input key to be transformed.
 * @return int The transformed output key.
 */
int EnigmaMachine::keyTransform(int input){
    return rotorBox.keyTransform(input);
}

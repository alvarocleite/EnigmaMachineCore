#pragma once

#include "../../RotorBox/include/RotorBox.hpp"
#include "../../PlugBoard/include/PlugBoard.hpp"

/**
 * @brief Class representing the Enigma machine.
 * This class encapsulates the functionality of the Enigma machine, including the rotor box
 * and the transformation of input keys through the rotors and reflector.
 */
class EnigmaMachine{
private:
    RotorBox rotorBox;
    PlugBoard plugBoard; // Optional: if you want to include a plugboard for additional transformations
public:
    EnigmaMachine();
    EnigmaMachine(int nRotorCount, const std::vector<int> &rotorPositions, const std::vector<std::string> &transformerFiles);
    EnigmaMachine(int nRotorCount, const std::vector<int> &rotorPositions, const std::vector<std::string> &transformerFiles, const std::array<Pair_t, PLUGBOARD_MAX_PAIRS> &plugBoardPairs);
    ~EnigmaMachine();

    int keyTransform(int input);
};


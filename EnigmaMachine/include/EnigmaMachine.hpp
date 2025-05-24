#pragma once

#include "../../RotorBox/include/RotorBox.hpp"

/**
 * @brief Class representing the Enigma machine.
 * This class encapsulates the functionality of the Enigma machine, including the rotor box
 * and the transformation of input keys through the rotors and reflector.
 */
class EnigmaMachine{
private:
    RotorBox rotorBox;
public:
    EnigmaMachine();
    EnigmaMachine(int nRotorCount, const std::vector<int> &rotorPositions, const std::vector<std::string> &transformerFiles);
    ~EnigmaMachine();

    int keyTransform(int input);
};


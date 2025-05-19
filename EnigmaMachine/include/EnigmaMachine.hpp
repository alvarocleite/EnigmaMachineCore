#pragma once

#include "../../RotorBox/include/RotorBox.hpp"

class EnigmaMachine{
private:
    RotorBox rotorBox;
public:
    EnigmaMachine();
    EnigmaMachine(int nRotorCount, const std::vector<int> &rotorPositions, const std::vector<std::string> &transformerFiles);
    ~EnigmaMachine();

    int keyTransform(int input);
};


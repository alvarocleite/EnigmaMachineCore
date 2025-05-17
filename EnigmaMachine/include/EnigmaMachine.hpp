#pragma once

#include "../../RotorBox/include/RotorBox.hpp"

class EnigmaMachine{
private:
    RotorBox rotorBox;
public:
    EnigmaMachine();
    ~EnigmaMachine();

    int keyTransform(int input);
};


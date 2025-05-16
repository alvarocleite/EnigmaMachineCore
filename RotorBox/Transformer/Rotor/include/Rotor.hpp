#pragma once

#include "../../include/Transformer.hpp"

class Rotor: public Transformer {
private:
    int notchPosition;
    int rotorRotationCount;
    bool initTransformLUT();
    int initRotorPosition(int offset = 0);
    inline bool isNotchPosition(int position);
public:
    Rotor();
    ~Rotor();
    int transform(int position, bool reverse = false);
    int rotate();
};

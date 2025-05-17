#pragma once

#include <string>
#include "../../include/Transformer.hpp"

class Rotor: public Transformer {
private:
    int notchPosition;
    int rotorRotationCount;
    bool initTransformLUT(std::string fileName);
    int initRotorPosition(int offset = 0);
    inline bool isNotchPosition(int position);
public:
    Rotor(std::string fileName);
    ~Rotor();
    int transform(int position, bool reverse = false);
    int rotate();
};

#pragma once

#include <string>
#include "../../include/Transformer.hpp"

/**
 * @brief Class representing a rotor in the Enigma machine.
 * This class inherits from the Transformer class and implements the functionality
 * for transforming positions based on the rotor's transformation lookup table (LUT).
 * It also handles the rotor's rotation and notch position.
 */
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

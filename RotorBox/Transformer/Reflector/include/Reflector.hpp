#pragma once

#include "../../include/Transformer.hpp"

/**
 * @brief Class representing a reflector in the Enigma machine.
 * This class inherits from the Transformer class and implements the functionality
 * for transforming positions based on the reflector's transformation lookup table (LUT).
 * Reflectors do not rotate, so the rotate function always returns 0.
 */
class Reflector : public Transformer {
private:
    bool initTransformLUT(std::string fileName);
public:
    Reflector(std::string fileName);
    ~Reflector();
    int transform(int position, bool reverse = false);
    int rotate() { return 0; } // Reflectors do not rotate
};

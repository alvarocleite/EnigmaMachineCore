#pragma once

#include <vector>
#include <memory>

#include "../Transformer/include/Transformer.hpp"

/**
 * @brief Class representing a box of rotors in the Enigma machine.
 * This class manages multiple rotors and a reflector, allowing for the transformation of input keys.
 * It handles the initialization of rotors, their positions, and the transformation process.
 */
class RotorBox{
private:
    int nRotorCount;
    std::vector<int> rotorPositions;
    std::vector<std::unique_ptr<Transformer>> transformerVec;
    int initTransformerVec(int nRotorCount, const std::vector<std::string> &rotorFiles);
    int updateRotors();
public:
    RotorBox();
    RotorBox(int nRotorCount, const std::vector<int> &rotorPositions, const std::vector<std::string> &rotorFiles);
    ~RotorBox();
    void printTransformerVec();

    int keyTransform(int input);
};

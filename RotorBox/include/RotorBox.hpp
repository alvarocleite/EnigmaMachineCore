#pragma once

#include <vector>
#include <memory>

#include "Transformer.hpp"

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

    /**
     * @brief Initializes the transformer vector with rotors and a reflector.
     * 
     * @param nRotorCount The number of rotors to be initialized.
     * @param rotorFiles A vector containing the file names for each rotor and reflector.
     * @throws std::runtime_error If initialization fails.
     */
    void initTransformerVec(int nRotorCount, const std::vector<std::string> &rotorFiles);

    /**
     * @brief Updates the positions of the rotors.
     */
    void updateRotors();

public:
    /**
     * @brief Constructor for the RotorBox class.
     * Initializes the rotor box with a default number of rotors (3) and their positions (all set to 0).
     * Also initializes the transformer vector with default rotor and reflector files.
     * 
     * @throws std::runtime_error If initialization of transformers fails.
     */
    RotorBox();

    /**
     * @brief Constructor for the RotorBox class.
     * Initializes the rotor box with a specified number of rotors, their positions, and corresponding files.
     * 
     * @param nRotorCount The number of rotors in the rotor box.
     * @param rotorPositions A vector containing the initial positions of each rotor.
     * @param rotorFiles A vector containing the file names for each rotor and reflector.
     * @throws std::invalid_argument If the number of rotors does not match the number of positions.
     * @throws std::runtime_error If initialization of transformers fails or file count is insufficient.
     */
    RotorBox(int nRotorCount, const std::vector<int> &rotorPositions, const std::vector<std::string> &rotorFiles);
    ~RotorBox() = default;

    /**
     * @brief Prints the types of transformers in the transformer vector.
     * This function iterates through the transformer vector and prints the type of each transformer.
     */
    void printTransformerVec();

    /**
     * @brief Transforms the input key through the rotor box.
     * 
     * @param input The input key to be transformed.
     * @return int The transformed output key.
     */
    int keyTransform(int input);
};

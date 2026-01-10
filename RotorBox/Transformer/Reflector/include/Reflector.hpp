#pragma once

#include "Transformer.hpp"

/**
 * @brief Class representing a reflector in the Enigma machine.
 * This class inherits from the Transformer class and implements the functionality
 * for transforming positions based on the reflector's transformation lookup table (LUT).
 * Reflectors do not rotate, so the rotate function always returns 0.
 */
class Reflector : public Transformer {
private:
    /** 
     * @brief Initializes the transformation lookup table (LUT) for the reflector.
     * 
     * @param fileName The name of the file containing the transformation data.
     * @return bool Returns true if the initialization is successful, false otherwise.
     */
    bool initTransformLUT(std::string fileName);

    /**
     * @brief Parses the reflector configuration from a TOML file.
     * 
     * @param fileName The path to the configuration file.
     * @return bool Returns true if parsing is successful, false otherwise.
     */
    bool parseConfig(std::string fileName);
public:
    /**
     * @brief Constructor for the Reflector class.
     * Initializes the reflector with a transformation lookup table (LUT) from a file.
     * Sets the type of the transformer to Reflector.
     */
    Reflector(std::string fileName);
    ~Reflector();

    /**
     * @brief Transforms the given position using the reflector's transformation lookup table (LUT).
     * 
     * @param position The input position to be transformed.
     * @param reverse It should not be set to true.
     * @return int The transformed position. Returns "-1" if the position is not found in the LUT when reverse is true.
     */
    int transform(int position, bool reverse = false) override;

    /**
     * @brief Reflector does not rotate.
     * 
     * @return int Always returns 0.
     */
    int rotate() override { return 0; }
};

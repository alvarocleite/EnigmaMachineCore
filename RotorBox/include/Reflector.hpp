/**
 * @file
 * @brief Header file for the Reflector class.
 */

#pragma once

#include "EnigmaMachineConfig.hpp"  // For ReflectorConfig
#include "Transformer.hpp"

/**
 * @brief Class representing a reflector in the Enigma machine.
 * This class inherits from the Transformer class and implements the functionality
 * for transforming positions based on the reflector's transformation lookup table (LUT).
 * Reflectors do not rotate, so the rotate function always returns 0.
 */
class Reflector : public Transformer {
public:
    /**
     * @brief Constructor for the Reflector class.
     * Initializes the reflector with a configuration.
     * Sets the type of the transformer to Reflector.
     *
     * @param config The ReflectorConfig structure containing the wiring map.
     */
    Reflector(const ReflectorConfig& config);
    ~Reflector() = default;

    /**
     * @brief Transforms the given position using the reflector's transformation lookup table (LUT).
     *
     * @param position The input position to be transformed.
     * @param reverse It should not be set to true.
     * @return int The transformed position. Returns "-1" if the position is not found in the LUT when reverse is true.
     * @details This method is const-qualified, ensuring that the signal transformation does not modify the internal state of the reflector.
     */
    int transform(int position, bool reverse = false) const override;

    /**
     * @brief Reflector does not rotate.
     *
     * @return int Always returns 0.
     */
    int rotate() override { return 0; }
};
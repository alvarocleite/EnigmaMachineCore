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
    explicit Reflector(const ReflectorConfig& config);
    ~Reflector() override = default;

    /**
     * @brief Transforms the given position using the reflector's transformation lookup table (LUT).
     *
     * @param position The input position to be transformed.
     * @param reverse It should not be set to true.
     * @return AlphabetIndex The transformed position. Returns "-1" if the position is not found in the LUT when reverse
     * is true.
     * @details This method is const-qualified, ensuring that the signal transformation does not modify the internal
     * state of the reflector.
     */
    AlphabetIndex transform(AlphabetIndex position, bool reverse = false) const override;

    /**
     * @brief Transforms the given position forward.
     * @param position The current position (0 to TRANSFORMER_SIZE - 1).
     * @return AlphabetIndex The transformed position (0 to TRANSFORMER_SIZE - 1).
     * @details Implements the signal reflection at the end of the rotor sequence.
     */
    AlphabetIndex transformForward(AlphabetIndex position) const override;

    /**
     * @brief Transforms the given position in reverse.
     * @param position The current position (0 to TRANSFORMER_SIZE - 1).
     * @return AlphabetIndex Returns -1 since reflectors are symmetric and do not have separate reverse paths.
     */
    AlphabetIndex transformReverse(AlphabetIndex position) const override;

    /**
     * @brief Reflector does not rotate.
     *
     * @return int Always returns 0.
     */
    int rotate() override { return 0; }
};
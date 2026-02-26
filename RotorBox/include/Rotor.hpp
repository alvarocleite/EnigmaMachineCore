#pragma once

#include "EnigmaMachineConfig.hpp"  // For RotorConfig
#include "Transformer.hpp"

/**
 * @file Rotor.hpp
 * @brief Defines the Rotor class, a concrete implementation of the Transformer interface.
 */

/**
 * @brief Class representing a rotor in the Enigma machine.
 * This class inherits from the Transformer class and implements the functionality
 * for transforming positions based on the rotor's transformation lookup table (LUT).
 * It also handles the rotor's rotation and notch position.
 */
class Rotor : public Transformer {
private:
    int notchPosition;
    int rotationCount;

    /**
     * @brief Generates the reverse transformation lookup table.
     * @throws std::runtime_error If reverse mapping generation fails.
     */
    void initReverseLookupTable();

    /**
     * @brief Initializes the rotor position.
     * This function sets the initial position of the rotor based on the provided offset.
     *
     * @param offset The initial offset for the rotor position.
     * @return int Returns 0 on success.
     */
    int initRotorPosition(int offset = 0);

public:
    /**
     * @brief Constructor for the Rotor class.
     * Initializes the rotor with a configuration.
     *
     * @param config The RotorConfig structure containing wiring and notch info.
     */
    Rotor(const RotorConfig& config);
    ~Rotor() = default;

    /**
     * @brief Transforms the given position based on the transformation lookup table (LUT).
     *
     * @param position The current position in the rotor.
     * @param reverse If true, transforms using the reverse LUT; otherwise, uses the forward LUT.
     * @return int The transformed position.
     */
    int transform(int position, bool reverse = false) override;

    /**
     * @brief Rotates the rotor by one position.
     * If the rotor reaches the notch position, it returns 1, indicating that the next rotor should rotate.
     * Otherwise, it returns 0.
     *
     * @return int Returns 1 if the rotor reached the notch position, otherwise returns 0.
     */
    int rotate() override;

    /**
     * @brief Sets the rotor to a specific position.
     *
     * @param position The position to set the rotor to (0-25).
     */
    void setPosition(int position) override;

    /**
     * @brief Gets the current position of the rotor.
     *
     * @return int The current position (0-25).
     */
    int getPosition() const override;

    /**
     * @brief Checks if the given position is the notch position.
     *
     * @param position The position to check.
     * @return true if the position is the notch position, false otherwise.
     */
    bool isNotchPosition(int position);
};
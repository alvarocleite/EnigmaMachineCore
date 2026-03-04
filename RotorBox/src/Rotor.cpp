/**
 * @file Rotor.cpp
 * @brief Implementation of the Rotor class.
 */

#include "Rotor.hpp"

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <vector>

/**
 * @details Initializes the Rotor by:
 * 1. Setting the notch position and initial rotation count.
 * 2. Validating the wiring size against TRANSFORMER_SIZE.
 * 3. Populating the forward transformation table.
 * 4. Generating the reverse transformation table via `initReverseLookupTable`.
 *
 * @throws std::runtime_error If the wiring size in the config is incorrect.
 */
Rotor::Rotor(const RotorConfig& config) {
    notchPosition = config.notchPosition;
    rotationCount = 0;
    type = TransformerType::Rotor;

    // Set forward wiring from config
    if (config.wiring.size() != TRANSFORMER_SIZE) {
        throw std::runtime_error("Rotor wiring size mismatch");
    }

    for (size_t i = 0; i < config.wiring.size(); ++i) {
        setTransformValue(0, static_cast<int>(i), config.wiring[i]);
    }

    initReverseLookupTable();
    initRotorPosition();
}

/**
 * @details Generates the mathematical inverse of the forward wiring to handle the return signal.
 * @internal If Forward(X) = Y, then Reverse(Y) = X. This ensures the signal can traverse
 * back through the rotors after being reflected. It iterates through the forward LUT
 * to map outputs back to inputs.
 */
void Rotor::initReverseLookupTable() {
    const auto& forwardRow = getTransformRow(0);
    std::vector<bool> seen(TRANSFORMER_SIZE, false);

    int newVal{0};
    std::for_each(forwardRow.begin(), forwardRow.end(), [&](int value) {
        if (value < 0 || value >= TRANSFORMER_SIZE) {
            throw std::runtime_error("Invalid Rotor mapping: Value out of range: " + std::to_string(value));
        }
        if (seen[value]) {
            throw std::runtime_error("Invalid Rotor mapping: Duplicate output value: " + std::to_string(value));
        }
        seen[value] = true;
        auto newCol = value;
        setTransformValue(1, /* col = */ newCol, /* value = */ newVal++);  // Initialize reverse mapping
    });

    auto missing_it = std::find(seen.begin(), seen.end(), false);
    if (missing_it != seen.end()) {
        throw std::runtime_error("Invalid Rotor mapping: Not a bijection. Missing output: " +
                                 std::to_string(std::distance(seen.begin(), missing_it)));
    }
}

/**
 * @details Sets the internal rotation counter to the specified offset.
 */
int Rotor::initRotorPosition(int offset) {
    rotationCount = offset;
    return 0;
}

bool Rotor::isNotchPosition(int position) const { return (position == notchPosition); }

/**
 * @details Handles the relative coordinate shift caused by the rotor's rotation.
 * 1. Shift input relative to rotor position.
 * 2. Apply internal wiring permutation.
 * 3. Shift output back to absolute machine coordinate system.
 *
 * @internal This logic ensures that the physical rotation of the rotor
 * effectively changes the entry and exit pins of the signal.
 */
int Rotor::transform(int position, bool reverse) const {
    return reverse ? transformReverse(position) : transformForward(position);
}

/**
 * @details Performs the forward signal pass (right-to-left).
 * 1. Adjust input position by current rotation offset.
 * 2. Look up the internal wiring map (row 0).
 * 3. Adjust output back to the machine's absolute reference frame.
 */
int Rotor::transformForward(int position) const {
    position = (position + rotationCount);
    if (position >= TRANSFORMER_SIZE) {
        position -= TRANSFORMER_SIZE;
    }
    position = getTransformValue(0, position);
    position = (position - rotationCount + TRANSFORMER_SIZE);
    if (position >= TRANSFORMER_SIZE) {
        position -= TRANSFORMER_SIZE;
    }

    return position;
}

/**
 * @details Performs the reverse signal pass (left-to-right).
 * 1. Adjust input position by current rotation offset.
 * 2. Look up the internal wiring map (row 1 - inverse).
 * 3. Adjust output back to the machine's absolute reference frame.
 */
int Rotor::transformReverse(int position) const {
    position = (rotationCount + position);
    if (position >= TRANSFORMER_SIZE) {
        position -= TRANSFORMER_SIZE;
    }
    position = getTransformValue(1, position);
    position = (position - rotationCount + TRANSFORMER_SIZE);
    if (position >= TRANSFORMER_SIZE) {
        position -= TRANSFORMER_SIZE;
    }

    return position;
}

/**
 * @details Increments the rotor's position by one step (modulo TRANSFORMER_SIZE).
 * Checks if the new position corresponds to the notch position.
 *
 * @return 1 if the rotor is now at the notch position (triggering a carry), 0 otherwise.
 */
int Rotor::rotate() {
    rotationCount = (rotationCount + 1);
    if (rotationCount >= TRANSFORMER_SIZE) {
        rotationCount -= TRANSFORMER_SIZE;
    }
    return isNotchPosition(rotationCount) ? 1 : 0;
}

/**
 * @details Manually sets the current rotational position of the rotor.
 * This is used for setting up the initial machine state key.
 */
void Rotor::setPosition(int position) { rotationCount = position; }

int Rotor::getPosition() const { return rotationCount; }
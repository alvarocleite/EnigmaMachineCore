#include "Rotor.hpp"

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>
#include <toml.hpp>

Rotor::Rotor(std::string fileName) {
    notchPosition = 0;
    rotorRotationCount = 0;
    type = TransformerType::Rotor;
    initTransformLUT(fileName);
    initRotorPosition();
}

/**
 * @details Initializes the rotor's wiring configuration.
 * This function orchestrates the initialization process by delegating the
 * configuration parsing to parseConfig and the inverse table generation
 * to initReverseTransformLUT.
 */
void Rotor::initTransformLUT(std::string fileName) {
    parseConfig(fileName);
    initReverseTransformLUT();
}

/**
 * @details Parses the rotor's wiring and notch position from a TOML file.
 * @internal This method enforces strict size and type checking to ensure
 * the configuration matches the expected transformer size and type.
 */
void Rotor::parseConfig(std::string fileName) {
    toml::value data;
    parseBasicConfig(fileName, "rotor", data);

    try {
        this->notchPosition = toml::find<int>(data, "rotor", "notchPosition");

        auto arr = toml::find<std::vector<int>>(data, "rotor", "forward");
        if (arr.size() != TRANSFORMER_SIZE) {
            throw std::runtime_error("TOML array size mismatch");
        }

        for (size_t i = 0; i < arr.size(); ++i) {
            setTransformValue(0, i, arr[i]);
        }

    } catch (const std::exception& e) {
        throw std::runtime_error("TOML parse error: " + std::string(e.what()));
    }
}

/**
 * @details Generates the mathematical inverse of the forward wiring to handle the return signal.
 * @internal If Forward(X) = Y, then Reverse(Y) = X. This ensures the signal can traverse
 * back through the rotors after being reflected. It iterates through the forward LUT
 * to map outputs back to inputs.
 */
void Rotor::initReverseTransformLUT() {
    const auto& forwardRow = getTransformRow(0);

    for (int forwardValue = 0; forwardValue < TRANSFORMER_SIZE; forwardValue++) {
        auto it = std::find_if(forwardRow.begin(), forwardRow.end(),
                               [forwardValue](int value) { return value == forwardValue; });

        if (it != forwardRow.end()) {
            int reverseIndex = std::distance(forwardRow.begin(), it);
            setTransformValue(1, forwardValue, reverseIndex);
        } else {
            // This implies the forward mapping is not a bijection (valid permutation)
            throw std::runtime_error("Invalid Rotor mapping: Not a bijection. Missing output: " +
                                     std::to_string(forwardValue));
        }
    }
}

int Rotor::initRotorPosition(int offset) {
    rotorRotationCount = offset;
    return 0;
}

inline bool Rotor::isNotchPosition(int position) { return (position == notchPosition); }

/**
 * @details Handles the relative coordinate shift caused by the rotor's rotation.
 * 1. Shift input relative to rotor position.
 * 2. Apply internal wiring permutation.
 * 3. Shift output back to absolute machine coordinate system.
 *
 * @internal This logic ensures that the physical rotation of the rotor
 * effectively changes the entry and exit pins of the signal.
 */
int Rotor::transform(int position, bool reverse) {
    position = (position + rotorRotationCount) % TRANSFORMER_SIZE;
    position = getTransformValue((int)reverse, position);
    position = (position - rotorRotationCount + TRANSFORMER_SIZE) % TRANSFORMER_SIZE;

    return position;
}

int Rotor::rotate() {
    rotorRotationCount = (rotorRotationCount + 1) % TRANSFORMER_SIZE;
    return isNotchPosition(rotorRotationCount) ? 1 : 0;
}

void Rotor::setPosition(int position) { rotorRotationCount = position; }

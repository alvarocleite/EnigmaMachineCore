/**
 * @file
 * @brief Implementation of the RotorBox class.
 */

#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>

#include "Reflector.hpp"
#include "Rotor.hpp"
#include "RotorBox.hpp"
#include "config.hpp"

/**
 * @details Initializes a standard 3-rotor configuration with empty/default wiring.
 * Note: usage of this constructor is discouraged without subsequent initialization.
 */
RotorBox::RotorBox() { rotorCount = 0; }

/**
 * @details Initializes a custom rotor configuration.
 * Validates that the number of provided positions matches the rotor count before
 * proceeding with transformer initialization and position setting.
 */
RotorBox::RotorBox(int count, const std::vector<int>& rotorPositions, const std::vector<RotorConfig>& rotors,
                   const ReflectorConfig& reflector) {
    if (std::cmp_not_equal(count, rotorPositions.size())) {
        throw std::invalid_argument("Error: Number of rotors and number of rotor positions do not match.");
    }

    this->rotorCount = count;
    for (const auto& position : rotorPositions) {
        this->rotorPositions.push_back(position);
    }

    // Will throw if initialization fails
    initTransformers(count, rotors, reflector);

    for (int i = 0; i < count; i++) {
        transformers.at(i)->setPosition(this->rotorPositions.at(i));
    }
}

void RotorBox::registerObserver(IEnigmaObserver* observer) { observers.push_back(observer); }

void RotorBox::removeObserver(IEnigmaObserver* observer) {
    auto iterator = std::remove(observers.begin(), observers.end(), observer);
    observers.erase(iterator, observers.end());
}

/**
 * @details Populates the internal transformer vector with unique pointers to Rotor and Reflector objects.
 *
 * @internal This method enforces the architectural constraint that the transformer vector
 * must contain exactly rotorCount rotors followed by one reflector at the end.
 * Memory is managed via std::unique_ptr to ensure proper cleanup.
 */
void RotorBox::initTransformers(int count, const std::vector<RotorConfig>& rotors, const ReflectorConfig& reflector) {
    transformers.clear();
    transformers.reserve(count + 1);

    // Validate input size: n Rotors
    if (rotors.size() != (size_t)count) {
        throw std::runtime_error("Error: Mismatch between rotor count and provided configurations.");
    }

    for (const auto& rotorConfig : rotors) {
        transformers.push_back(std::make_unique<Rotor>(rotorConfig));
    }
    transformers.push_back(std::make_unique<Reflector>(reflector));
}

void RotorBox::printTransformers() const {
    for (auto& transformer : transformers) {
        std::cout << "Transformer Type: " << static_cast<int>(transformer->getType()) << "\n";
    }
}

/**
 * @details The signal path inside the RotorBox simulates the actual wiring:
 * 1. Mechanical Step: Rotate rotors BEFORE signal processing.
 * 2. Forward Pass: Right-to-Left from the entry wheel through all rotors.
 * 3. Reflector: Swaps the character and sends it back.
 * 4. Reverse Pass: Left-to-Right back through the rotors using inverse mappings.
 */
int RotorBox::keyTransform(int input) {
    updateRotors();

    // transform through rotors forward
    bool reverse = false;
    int newPosition = input;
    for (int i = 0; i < rotorCount; i++) {
        newPosition = transformers.at(i)->transform(newPosition, reverse);
    }

    // reflector
    newPosition = transformers.at(rotorCount)->transform(newPosition, reverse);

    // transform through rotors in reverse
    reverse = true;
    for (int i = rotorCount - 1; i >= 0; i--) {
        newPosition = transformers.at(i)->transform(newPosition, reverse);
    }

    return newPosition;
}

/**
 * @details Updates the rotor positions according to Enigma stepping mechanics.
 * 1. The rightmost rotor always steps.
 * 2. A rotor steps the next rotor to its left if it is at its notch position.
 * 3. Double-stepping occurs when a rotor steps due to its own notch position and
 *   also causes the next rotor to step.
 * After stepping, observers are notified of the new rotor positions.
 */
void RotorBox::updateRotors() {
    if (rotorCount < 1) return;

    // Storing notch states BEFORE stepping
    std::vector<bool> atNotch(rotorCount, false);
    for (int i = 0; i < rotorCount; i++) {
        auto* rotor = static_cast<Rotor*>(transformers.at(i).get());
        int pos = rotor->getPosition();
        atNotch[i] = rotor->isNotchPosition(pos);
    }

    //  Step rotors according to Enigma mechanics

    // Rightmost rotor always steps
    transformers.at(0)->rotate();

    // Remaining rotors
    for (int i = 1; i < rotorCount; i++) {
        bool carried = atNotch[i - 1];
        bool doubleStep = (i < rotorCount - 1) && atNotch[i];
        if (carried || doubleStep) {
            transformers.at(i)->rotate();
        }
    }

    // Notify observers
    for (int i = 0; i < rotorCount; i++) {
        int pos = transformers.at(i)->getPosition();
        for (auto* obs : observers) {
            obs->onRotorStepped(i, pos);
        }
    }
}
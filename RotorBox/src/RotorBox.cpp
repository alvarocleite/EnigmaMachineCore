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
RotorBox::RotorBox() { nRotorCount = 0; }

/**
 * @details Initializes a custom rotor configuration.
 * Validates that the number of provided positions matches the rotor count before
 * proceeding with transformer initialization and position setting.
 */
RotorBox::RotorBox(int nRotorCount, const std::vector<int>& rotorPositions, const std::vector<RotorConfig>& rotors,
                   const ReflectorConfig& reflector) {
    if (std::cmp_not_equal(nRotorCount, rotorPositions.size())) {
        throw std::invalid_argument("Error: Number of rotors and number of rotor positions do not match.");
    }

    this->nRotorCount = nRotorCount;
    for (const auto& position : rotorPositions) {
        this->rotorPositions.push_back(position);
    }

    // Will throw if initialization fails
    initTransformerVec(nRotorCount, rotors, reflector);

    for (int i = 0; i < nRotorCount; i++) {
        transformerVec.at(i)->setPosition(this->rotorPositions.at(i));
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
 * must contain exactly nRotorCount rotors followed by one reflector at the end.
 * Memory is managed via std::unique_ptr to ensure proper cleanup.
 */
void RotorBox::initTransformerVec(int nRotorCount, const std::vector<RotorConfig>& rotors,
                                  const ReflectorConfig& reflector) {
    transformerVec.clear();
    transformerVec.reserve(nRotorCount + 1);

    // Validate input size: n Rotors
    if (rotors.size() != (size_t)nRotorCount) {
        throw std::runtime_error("Error: Mismatch between rotor count and provided configurations.");
    }

    for (const auto& rotorConfig : rotors) {
        transformerVec.push_back(std::make_unique<Rotor>(rotorConfig));
    }
    transformerVec.push_back(std::make_unique<Reflector>(reflector));
}

void RotorBox::printTransformerVec() const {
    for (auto& transformer : transformerVec) {
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
    for (int i = 0; i < nRotorCount; i++) {
        newPosition = transformerVec.at(i)->transform(newPosition, reverse);
    }

    // reflector
    newPosition = transformerVec.at(nRotorCount)->transform(newPosition, reverse);

    // transform through rotors in reverse
    reverse = true;
    for (int i = nRotorCount - 1; i >= 0; i--) {
        newPosition = transformerVec.at(i)->transform(newPosition, reverse);
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
    if (nRotorCount < 1) return;

    // Storing notch states BEFORE stepping
    std::vector<bool> atNotch(nRotorCount, false);
    for (int i = 0; i < nRotorCount; i++) {
        auto* rotor = static_cast<Rotor*>(transformerVec.at(i).get());
        int pos = rotor->getPosition();
        atNotch[i] = rotor->isNotchPosition(pos);
    }

    //  Step rotors according to Enigma mechanics

    // Rightmost rotor always steps
    transformerVec.at(0)->rotate();

    // Remaining rotors
    for (int i = 1; i < nRotorCount; i++) {
        bool carried = atNotch[i - 1];
        bool doubleStep = (i < nRotorCount - 1) && atNotch[i];
        if (carried || doubleStep) {
            transformerVec.at(i)->rotate();
        }
    }

    // Notify observers
    for (int i = 0; i < nRotorCount; i++) {
        int pos = transformerVec.at(i)->getPosition();
        for (auto* obs : observers) {
            obs->onRotorStepped(i, pos);
        }
    }
}

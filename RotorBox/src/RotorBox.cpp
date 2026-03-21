/**
 * @file
 * @brief Implementation of the RotorBox class.
 */

#include <algorithm>
#include <iostream>
#include <ranges>
#include <stdexcept>
#include <string>
#include <utility>

#include "EnigmaConfig.hpp"
#include "Reflector.hpp"
#include "Rotor.hpp"
#include "RotorBox.hpp"

/**
 * @details Initializes a standard 3-rotor configuration with empty/default wiring.
 * Note: usage of this constructor is discouraged without subsequent initialization.
 */
RotorBox::RotorBox(ILogger* logger) : logger(logger) {
    rotorCount = 3;
    rotorPositions = std::vector<AlphabetIndex>(rotorCount, 0);

    std::vector<RotorConfig> defaultRotors(rotorCount);
    initTransformers(defaultRotors, ReflectorConfig{});
}

/**
 * @details Initializes a custom rotor configuration.
 * Validates that the number of provided positions matches the rotor count before
 * proceeding with transformer initialization and position setting.
 */
RotorBox::RotorBox(const std::vector<AlphabetIndex>& rotorPositions, const std::vector<RotorConfig>& rotors,
                   const ReflectorConfig& reflector, ILogger* logger)
    : logger(logger) {
    if (std::cmp_not_equal(rotorPositions.size(), rotors.size())) {
        throw std::invalid_argument("Error: Number of rotors and number of rotor positions do not match.");
    }

    rotorCount = (int)rotorPositions.size();
    this->rotorPositions = rotorPositions;

    initTransformers(rotors, reflector);

    for (int i = 0; i < rotorCount; i++) {
        transformers.at(i)->setPosition(this->rotorPositions.at(i));
    }
}

RotorBox::RotorBox(std::vector<AlphabetIndex>&& rotorPositions, std::vector<RotorConfig>&& rotors,
                   ReflectorConfig&& reflector, ILogger* logger)
    : rotorCount((int)rotorPositions.size()), rotorPositions(std::move(rotorPositions)), logger(logger) {
    if (std::cmp_not_equal(this->rotorPositions.size(), rotors.size())) {
        throw std::invalid_argument("Error: Number of rotors and number of rotor positions do not match.");
    }

    // Optimization: avoid copying configs by moving them into the transformers
    transformers.reserve(rotorCount + 1);
    for (auto& rotorConfig : rotors) {
        transformers.push_back(std::make_unique<Rotor>(std::move(rotorConfig)));
    }
    transformers.push_back(std::make_unique<Reflector>(std::move(reflector)));

    for (int i = 0; i < rotorCount; i++) {
        transformers.at(i)->setPosition(this->rotorPositions.at(i));
    }
}

void RotorBox::registerObserver(IEnigmaObserver* observer) { observers.push_back(observer); }

void RotorBox::removeObserver(IEnigmaObserver* observer) {
    auto iterator = std::ranges::remove(observers, observer).begin();
    observers.erase(iterator, observers.end());
}

/**
 * @details Populates the internal transformer vector with unique pointers to Rotor and Reflector objects.
 *
 * @internal This method enforces the architectural constraint that the transformer vector
 * must contain exactly rotorCount rotors followed by one reflector at the end.
 * Memory is managed via std::unique_ptr to ensure proper cleanup.
 */
void RotorBox::initTransformers(const std::vector<RotorConfig>& rotors, const ReflectorConfig& reflector) {
    transformers.clear();
    transformers.reserve(rotorCount + 1);

    for (const auto& rotorConfig : rotors) {
        transformers.push_back(std::make_unique<Rotor>(rotorConfig));
    }
    transformers.emplace_back(std::make_unique<Reflector>(reflector));
}

void RotorBox::printTransformers() const {
    if (logger) {
        for (size_t i = 0; i < transformers.size(); ++i) {
            std::string msg = "Transformer " + std::to_string(i) +
                              " Type: " + std::to_string(static_cast<int>(transformers[i]->getType()));
            logger->log(LogLevel::Info, msg);
        }
    }
}

/**
 * @details The signal path inside the RotorBox simulates the actual wiring:
 * 1. Mechanical Step: Rotate rotors BEFORE signal processing.
 * 2. Forward Pass: Right-to-Left from the entry wheel through all rotors.
 * 3. Reflector: Swaps the character and sends it back.
 * 4. Reverse Pass: Left-to-Right back through the rotors using inverse mappings.
 */
AlphabetIndex RotorBox::keyTransform(AlphabetIndex input) {
    updateRotors();

    AlphabetIndex newPosition = input;
    // transform through rotors forward
    for (const auto& transformer : transformers | std::views::take(rotorCount)) {
        newPosition = transformer->transformForward(newPosition);
    }

    // reflector
    newPosition = transformers.at(rotorCount)->transformForward(newPosition);

    // transform through rotors in reverse
    for (const auto& transformer : transformers | std::views::take(rotorCount) | std::views::reverse) {
        newPosition = transformer->transformReverse(newPosition);
    }

    return newPosition;
}

void RotorBox::setLogger(ILogger* log) { this->logger = log; }

/**
 * @details Updates the rotor positions according to Enigma stepping mechanics.
 * 1. The rightmost rotor always steps.
 * 2. A rotor steps the next rotor to its left if it is at its notch position.
 * 3. Double-stepping occurs when a rotor steps due to its own notch position and
 *   also causes the next rotor to step.
 * After stepping, observers are notified of the new rotor positions.
 */
void RotorBox::updateRotors() {
    if (rotorCount < 1) {
        return;
    }

    // Storing notch states BEFORE stepping
    std::vector<char> atNotch(rotorCount, 0);
    std::ranges::transform(transformers | std::views::take(rotorCount), atNotch.begin(), [](const auto& transformer) {
        const auto* rotor = static_cast<const Rotor*>(transformer.get());
        return rotor->isNotchPosition(rotor->getPosition()) ? 1 : 0;
    });

    //  Step rotors according to Enigma mechanics
    for (int i = 0; i < rotorCount; i++) {
        if (i == 0) {  // Rightmost rotor always steps (i == 0)
            transformers.at(i)->rotate();
            if (logger) logger->log(LogLevel::Debug, "Rotor 0 stepped.");
            continue;
        }
        bool carried = atNotch[i - 1] != 0;
        bool doubleStep = (i < rotorCount - 1) && (atNotch[i] != 0);
        if (carried || doubleStep) {
            transformers.at(i)->rotate();
            if (logger) {
                std::string reason = carried ? "carry" : "double-step";
                logger->log(LogLevel::Debug, "Rotor " + std::to_string(i) + " stepped due to " + reason + ".");
            }
        }
    }

    // Notify observers
    std::ranges::for_each(std::views::iota(0, rotorCount), [this](int index) {
        const int position = transformers.at(index)->getPosition();
        std::ranges::for_each(observers,
                              [index, position](auto* observer) { observer->onRotorStepped(index, position); });
    });
}

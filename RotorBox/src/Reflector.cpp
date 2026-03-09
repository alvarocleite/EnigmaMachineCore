/**
 * @file
 * @brief Implementation of the Reflector class.
 */

#include "Reflector.hpp"
#include <iostream>
#include <stdexcept>
#include <vector>

Reflector::Reflector(const ReflectorConfig& config) {
    type = TransformerType::Reflector;

    if (config.wiring.size() != TRANSFORMER_SIZE) {
        throw std::runtime_error("Reflector wiring size mismatch");
    }

    for (size_t i = 0; i < config.wiring.size(); ++i) {
        setTransformValue(0, static_cast<int>(i), config.wiring[i]);
    }

    // Initialize reverse transformation vector to -1.
    // Reflectors are typically symmetric, but the Transformer base class supports separate reverse path.
    // For a reflector, the return path is usually implicit in the map itself if symmetric.
    // However, keeping consistent with the old code:
    fillTransformRow(1, -1);
}

/**
 * @details Performs the signal reflection.
 * @internal The 'reverse' parameter is ignored for reflectors as they
 * occupy the 'turn-around' point in the signal path.
 */
AlphabetIndex Reflector::transform(AlphabetIndex position, bool reverse) const {
    return reverse ? transformReverse(position) : transformForward(position);
}

/**
 * @details Reflects the input signal back towards the rotors.
 * @internal This operation uses the primary transformation mapping (row 0).
 */
AlphabetIndex Reflector::transformForward(AlphabetIndex position) const { return getTransformValue(0, position); }

/**
 * @details Returns -1 for Reflector reverse transformations.
 * @internal Reflectors are unidirectional; the signal only enters from the forward side.
 */
AlphabetIndex Reflector::transformReverse(AlphabetIndex position) const { return getTransformValue(1, position); }
#pragma once

#include "EnigmaTypes.hpp"

/**
 * @file
 * @brief Header file for the PlugBoardPair struct.
 */

/**
 * @brief Represents a pair of characters connected on the plugboard.
 */
struct PlugBoardPair {
    /** @brief The first port index (0 - (TRANSFORMER_SIZE - 1)). */
    AlphabetIndex sourcePortIndex{-1};
    /** @brief The second port index (0 - (TRANSFORMER_SIZE - 1)). */
    AlphabetIndex destinationPortIndex{-1};
};

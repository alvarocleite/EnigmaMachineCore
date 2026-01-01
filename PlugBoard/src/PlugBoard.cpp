#include "PlugBoard.hpp"
#include "config.hpp"
#include <iostream>

/**
 * @brief Constructor for the PlugBoard class.
 * Initializes an empty plugboard with no pairs (identity mapping).
 */
PlugBoard::PlugBoard() {
    for (int i = 0; i < TRANSFORMER_SIZE; ++i) {
        mapping[i] = i;
    }
}

/**
 * @brief Constructor for the PlugBoard class.
 * Initializes the plugboard with a given array of pairs.
 * Validates that ports are not already used before connecting.
 * 
 * @param pairs An array of pairs to initialize the plugboard with.
 */
PlugBoard::PlugBoard(std::array<Pair_t, PLUGBOARD_MAX_PAIRS> pairs) : PlugBoard() {
    for (const auto& pair : pairs) {
        int a = pair.a;
        int b = pair.b;

        if (a < 0 || a >= TRANSFORMER_SIZE || b < 0 || b >= TRANSFORMER_SIZE) {
            continue;
        }

        if (a == b) {
            continue;
        }

        if (mapping[a] != a || mapping[b] != b) {
            std::cerr << "Warning: PlugBoard conflict for pair (" << a << ", " << b << "). Skipping." << std::endl;
            continue;
        }

        mapping[a] = b;
        mapping[b] = a;
    }
}

PlugBoard::~PlugBoard() {}

/**
 * @brief Swaps the input key based on the plugboard pairs.
 * Uses a direct lookup table for O(1) performance.
 * 
 * @param key The input key to be swapped.
 * @return int The swapped key.
 */
int PlugBoard::swap(int key) const {
    if (key < 0 || key >= TRANSFORMER_SIZE) {
        return key; 
    }
    return mapping[key];
}
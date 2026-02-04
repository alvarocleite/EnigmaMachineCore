/**
 * @file
 * @brief Implementation of the PlugBoard class.
 */

#include "PlugBoard.hpp"
#include <iostream>
#include <stdexcept>
#include <string>
#include "config.hpp"

PlugBoard::PlugBoard() {
    for (int i = 0; i < TRANSFORMER_SIZE; ++i) {
        mapping.at(i) = i;
    }
}

/**
 * @details Validates the provided pairs before mapping them.
 * A port is 'unused' if it maps to itself.
 * If either port 'a' or 'b' is already mapped to something else, a conflict is reported
 * because a socket cannot have two plugs.
 *
 * @throws std::invalid_argument If a port index is out of range or if there is a mapping conflict.
 */
PlugBoard::PlugBoard(std::array<PlugBoardPair, PLUGBOARD_MAX_PAIRS> pairs) : PlugBoard() {
    for (const auto& pair : pairs) {
        int a = pair.sourcePortIndex;
        int b = pair.destinationPortIndex;

        // Skip uninitialized/empty pairs
        if (a == -1 || b == -1) {
            continue;
        }

        if (a < 0 || a >= TRANSFORMER_SIZE || b < 0 || b >= TRANSFORMER_SIZE) {
            throw std::invalid_argument("PlugBoard error: Port index out of range (" + std::to_string(a) + ", " +
                                        std::to_string(b) + ").");
        }

        if (a == b) {
            continue;
        }

        if (mapping.at(a) != a || mapping.at(b) != b) {
            throw std::invalid_argument("PlugBoard error: Conflict for pair (" + std::to_string(a) + ", " +
                                        std::to_string(b) + "). Port already in use.");
        }

        mapping.at(a) = b;
        mapping.at(b) = a;
    }
}

/**
 * @details Performs a character swap using the pre-calculated mapping table.
 * @internal This operation is O(1) and is performed twice for every key press in the EnigmaMachine.
 */
int PlugBoard::swap(int key) const {
    if (key < 0 || key >= TRANSFORMER_SIZE) {
        return key;
    }
    return mapping.at(key);
}
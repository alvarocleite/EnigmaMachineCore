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
    for (const auto& [sourcePortIndex, destinationPortIndex] : pairs) {

        // Skip uninitialized/empty pairs
        if (sourcePortIndex == -1 || destinationPortIndex == -1) {
            continue;
        }

        if (sourcePortIndex < 0 || sourcePortIndex >= TRANSFORMER_SIZE || destinationPortIndex < 0 || destinationPortIndex >= TRANSFORMER_SIZE) {
            throw std::invalid_argument("PlugBoard error: Port index out of range (" + std::to_string(sourcePortIndex) + ", " +
                                        std::to_string(destinationPortIndex) + ").");
        }

        if (sourcePortIndex == destinationPortIndex) {
            continue;
        }

        if (mapping.at(sourcePortIndex) != sourcePortIndex || mapping.at(destinationPortIndex) != destinationPortIndex) {
            throw std::invalid_argument("PlugBoard error: Conflict for pair (" + std::to_string(sourcePortIndex) + ", " +
                                        std::to_string(destinationPortIndex) + "). Port already in use.");
        }

        mapping.at(sourcePortIndex) = destinationPortIndex;
        mapping.at(destinationPortIndex) = sourcePortIndex;
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
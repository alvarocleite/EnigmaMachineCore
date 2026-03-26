/**
 * @file
 * @brief Implementation of the PlugBoard class.
 */

#include "PlugBoard.hpp"
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <string>
#include "EnigmaConfig.hpp"

PlugBoard::PlugBoard() { std::iota(mapping.begin(), mapping.end(), 0); }

/**
 * @details Validates the provided pairs before mapping them.
 * A port is 'unused' if it maps to itself.
 * If either port 'a' or 'b' is already mapped to something else, a conflict is reported
 * because a socket cannot have two plugs.
 *
 * @throws std::invalid_argument If a port index is out of range or if there is a mapping conflict.
 */
PlugBoard::PlugBoard(const std::array<PlugBoardPair, enigma::MAX_PLUGBOARD_PAIRS>& pairs) : PlugBoard() {
    for (const auto& [sourcePortIndex, destinationPortIndex] : pairs) {
        // Skip uninitialized/empty pairs
        if (sourcePortIndex == -1 || destinationPortIndex == -1) {
            continue;
        }

        if (sourcePortIndex < 0 || sourcePortIndex >= enigma::TRANSFORMER_SIZE || destinationPortIndex < 0 ||
            destinationPortIndex >= enigma::TRANSFORMER_SIZE) {
            throw std::invalid_argument("PlugBoard error: Port index out of range (" + std::to_string(sourcePortIndex) +
                                        ", " + std::to_string(destinationPortIndex) + ").");
        }

        if (sourcePortIndex == destinationPortIndex) {
            continue;
        }

        if (mapping.at(sourcePortIndex) != sourcePortIndex ||
            mapping.at(destinationPortIndex) != destinationPortIndex) {
            throw std::invalid_argument("PlugBoard error: Conflict for pair (" + std::to_string(sourcePortIndex) +
                                        ", " + std::to_string(destinationPortIndex) + "). Port already in use.");
        }

        mapping.at(sourcePortIndex) = destinationPortIndex;
        mapping.at(destinationPortIndex) = sourcePortIndex;
    }
}

namespace {

enigma::EnigmaError validatePlugBoardPairs(const std::array<PlugBoardPair, enigma::MAX_PLUGBOARD_PAIRS>& pairs,
                                           std::array<AlphabetIndex, enigma::TRANSFORMER_SIZE>& mapping) {
    for (const auto& [sourcePortIndex, destinationPortIndex] : pairs) {
        if (sourcePortIndex == -1 || destinationPortIndex == -1) {
            continue;
        }

        if (sourcePortIndex < 0 || sourcePortIndex >= enigma::TRANSFORMER_SIZE || destinationPortIndex < 0 ||
            destinationPortIndex >= enigma::TRANSFORMER_SIZE) {
            return enigma::EnigmaError::PlugBoardPortOutOfRange;
        }

        if (sourcePortIndex == destinationPortIndex) {
            continue;
        }

        if (mapping.at(sourcePortIndex) != sourcePortIndex ||
            mapping.at(destinationPortIndex) != destinationPortIndex) {
            return enigma::EnigmaError::PlugBoardPortConflict;
        }

        mapping.at(sourcePortIndex) = destinationPortIndex;
        mapping.at(destinationPortIndex) = sourcePortIndex;
    }
    return enigma::EnigmaError::None;
}

}  // namespace

enigma::Result<PlugBoard> PlugBoard::create(const std::array<PlugBoardPair, enigma::MAX_PLUGBOARD_PAIRS>& pairs) {
    std::array<AlphabetIndex, enigma::TRANSFORMER_SIZE> mapping;
    std::iota(mapping.begin(), mapping.end(), 0);

    auto error = validatePlugBoardPairs(pairs, mapping);
    if (error != enigma::EnigmaError::None) {
        return nonstd::make_unexpected(error);
    }

    PlugBoard pb;
    pb.mapping = mapping;
    return pb;
}

/**
 * @details Performs a character swap using the pre-calculated mapping table.
 * @internal This operation is O(1) and is performed twice for every key press in the EnigmaMachine.
 */
AlphabetIndex PlugBoard::swap(AlphabetIndex key) const {
    if (key < 0 || key >= enigma::TRANSFORMER_SIZE) {
        return key;
    }
    return mapping.at(key);
}

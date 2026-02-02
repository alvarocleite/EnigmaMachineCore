/**
 * @file
 * @brief Header file for the PlugBoard class.
 */

#pragma once

#include <array>

#include "config.hpp"
#include "PlugBoardPair.hpp"

class PlugBoard {
private:
    std::array<int, TRANSFORMER_SIZE> mapping;  // Direct mapping: mapping[Input] = Output

public:
    /**
     * @brief Constructor for the PlugBoard class.
     * Initializes an empty plugboard with no pairs (identity mapping).
     */
    PlugBoard();

    /**
     * @brief Constructor for the PlugBoard class.
     * Initializes the plugboard with a given array of pairs.
     * Validates that ports are not already used before connecting.
     *
     * @param pairs An array of pairs to initialize the plugboard with.
     * @throws std::invalid_argument If a port index is out of range or if there is a mapping conflict.
     */
    PlugBoard(std::array<PlugBoardPair, PLUGBOARD_MAX_PAIRS> pairs);
    ~PlugBoard() = default;

    /**
     * @brief Swaps the input key based on the plugboard pairs.
     *
     * @param key The input key to be swapped.
     * @return int The swapped key.
     */
    int swap(int key) const;
};

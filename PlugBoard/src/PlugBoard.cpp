#include "../include/PlugBoard.hpp"
#include "../../config/config.hpp"

/**
 * @brief Constructor for the PlugBoard class.
 * Initializes an empty plugboard with no pairs.
 */
PlugBoard::PlugBoard() : pairs{}, pairCount(0) {}

/**
 * @brief Constructor for the PlugBoard class.
 * Initializes the plugboard with a given array of pairs, adding only valid pairs.
 * 
 * @param pairs An array of pairs to initialize the plugboard with.
 */
PlugBoard::PlugBoard(std::array<Pair_t, 10> pairs) {
    for (const auto& pair : pairs) {
        if (isPairValid(pair.a, pair.b)) {
            addPair(pair.a, pair.b);
        }
    }
}

PlugBoard::~PlugBoard() {}

/**
 * @brief Checks if a pair of values is valid for the plugboard.
 * 
 * @param a The first value of the pair.
 * @param b The second value of the pair.
 * @return true if the pair is valid, false otherwise.
 */
bool PlugBoard::isPairValid(int a, int b) {
    // Check if values are within valid range and not the same
    if (a < 0 || a >= TRANSFORMER_SIZE || b < 0 || b >= TRANSFORMER_SIZE) {
        return false; // Invalid pair
    }

    // Check if the values are not the same
    if (a == b) {
        return false; // Pair cannot have the same elements
    }

    // Check if each value of the pair already exists
    for (int i = 0; i < pairCount; i++) {
        if ((pairs[i].a == a && pairs[i].b == b) || (pairs[i].a == b && pairs[i].b == a)) {
            return false; // At least one of the values already exists
        }
    }

    return true;
}

/**
 * @brief Adds a pair of values to the plugboard.
 * 
 * @param a The first value of the pair.
 * @param b The second value of the pair.
 * @return true if the pair was added successfully, false otherwise.
 */
bool PlugBoard::addPair(int a, int b) {
    if (pairCount < pairs.size()) {
        pairs[pairCount++] = {a, b};
        return true; // Pair added successfully
    }
    return false; // No space to add more pairs
}

/**
 * @brief Swaps the input key based on the plugboard pairs.
 * 
 * @param key The input key to be swapped.
 * @return int The swapped key, or the original key if no swap is found.
 */
int PlugBoard::swap(int key) {
    for (const auto& pair : pairs) {
        if (pair.a == key) {
            return pair.b; // Return the paired value
        } 
        else if (pair.b == key) {
            return pair.a; // Return the paired value
        }
    }
    return key; // No swap, return the original key
}

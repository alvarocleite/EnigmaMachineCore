/**
 * @file
 * @brief Implementation of the Transformer class.
 */

#include "Transformer.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

Transformer::Transformer() { type = TransformerType::Undefined; }

int Transformer::sizeOfLookupTable() const {
    return static_cast<int>(lookupTable.size()) * static_cast<int>(lookupTable[0].size());
}

TransformerType Transformer::getType() const { return type; }

/**
 * @brief Sets a value in the transformation lookup table.
 */
void Transformer::setTransformValue(int row, int col, AlphabetIndex value) { lookupTable.at(row).at(col) = value; }

/**
 * @brief Gets a value from the transformation lookup table.
 */
AlphabetIndex Transformer::getTransformValue(int row, int col) const { return lookupTable.at(row).at(col); }

/**
 * @brief Fills a row of the transformation lookup table with a specific value.
 */
void Transformer::fillTransformRow(int row, AlphabetIndex value) { lookupTable.at(row).fill(value); }

/**
 * @brief Copies a whole array into a row of the transformation lookup table.
 */
void Transformer::copyTransformRow(int row, const std::array<AlphabetIndex, TRANSFORMER_SIZE>& values) {
    lookupTable.at(row) = values;
}

/**
 * @brief Gets a read-only reference to a row in the transformation lookup table.
 */
const std::array<AlphabetIndex, TRANSFORMER_SIZE>& Transformer::getTransformRow(int row) const {
    return lookupTable.at(row);
}

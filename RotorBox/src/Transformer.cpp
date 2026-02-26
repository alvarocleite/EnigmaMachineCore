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
void Transformer::setTransformValue(int row, int col, int value) { lookupTable.at(row).at(col) = value; }

/**
 * @brief Gets a value from the transformation lookup table.
 */
int Transformer::getTransformValue(int row, int col) const { return lookupTable.at(row).at(col); }

/**
 * @brief Fills a row of the transformation lookup table with a specific value.
 */
void Transformer::fillTransformRow(int row, int value) { lookupTable.at(row).fill(value); }

/**
 * @brief Gets a read-only reference to a row in the transformation lookup table.
 */
const std::array<int, TRANSFORMER_SIZE>& Transformer::getTransformRow(int row) const { return lookupTable.at(row); }
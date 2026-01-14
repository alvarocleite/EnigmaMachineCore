/**
 * @file
 * @brief Implementation of the Transformer class.
 */

#include "Transformer.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

Transformer::Transformer() { type = TransformerType::NotDefined; }

int Transformer::sizeOfTransformLUT() const { return transformLUT.size() * transformLUT[0].size(); }

TransformerType Transformer::getType() const { return type; }

/**
 * @brief Sets a value in the transformation lookup table.
 */
void Transformer::setTransformValue(int row, int col, int value) { transformLUT.at(row).at(col) = value; }

/**
 * @brief Gets a value from the transformation lookup table.
 */
int Transformer::getTransformValue(int row, int col) const { return transformLUT.at(row).at(col); }

/**
 * @brief Fills a row of the transformation lookup table with a specific value.
 */
void Transformer::fillTransformRow(int row, int value) { transformLUT.at(row).fill(value); }

/**
 * @brief Gets a read-only reference to a row in the transformation lookup table.
 */
const std::array<int, TRANSFORMER_SIZE>& Transformer::getTransformRow(int row) const { return transformLUT.at(row); }
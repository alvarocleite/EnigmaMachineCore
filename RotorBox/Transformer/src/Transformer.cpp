#include "Transformer.hpp"

#include <fstream>
#include <iostream>
#include <string>
#include <stdexcept>

Transformer::Transformer(){
    type = TransformerType::NotDefined;
}

Transformer::~Transformer(){}

int Transformer::sizeOfTransformLUT() const{
    return transformLUT.size() * transformLUT[0].size();
}

TransformerType Transformer::getType() const{
    return type;
}

/**
 * @details Performs the foundational parsing and validation for all transformer components.
 * 1. Attempts to parse the TOML file.
 * 2. Validates that the 'size' field matches the global TRANSFORMER_SIZE (usually 26).
 * 3. Validates that the component 'type' matches the expected derived class type.
 * 
 * @internal This method centralizes file I/O and common metadata validation to ensure 
 * that malformed or mismatched configuration files are caught before component-specific 
 * parsing begins. Errors are thrown as std::runtime_error.
 */
void Transformer::parseBasicConfig(std::string fileName, std::string expectedType, toml::value& outData){
    try {
        outData = toml::parse(fileName);
        
        auto size = toml::find<int>(outData, "size");
        if (size != TRANSFORMER_SIZE) {
            throw std::runtime_error("Transformer size mismatch: expected " + std::to_string(TRANSFORMER_SIZE) + ", got " + std::to_string(size));
        }

        auto typeStr = toml::find<std::string>(outData, "type");
        if (typeStr != expectedType) {
            throw std::runtime_error("Wrong config file: expected " + expectedType + ", got " + typeStr);
        }

    } catch(const std::exception& e) {
        throw std::runtime_error("TOML parse error in " + fileName + ": " + e.what());
    }
}

/**
 * @brief Sets a value in the transformation lookup table.
 */
void Transformer::setTransformValue(int row, int col, int value) {
    transformLUT.at(row).at(col) = value;
}

/**
 * @brief Gets a value from the transformation lookup table.
 */
int Transformer::getTransformValue(int row, int col) const {
    return transformLUT.at(row).at(col);
}

/**
 * @brief Fills a row of the transformation lookup table with a specific value.
 */
void Transformer::fillTransformRow(int row, int value) {
    transformLUT.at(row).fill(value);
}

/**
 * @brief Gets a read-only reference to a row in the transformation lookup table.
 */
const std::array<int, TRANSFORMER_SIZE>& Transformer::getTransformRow(int row) const {
    return transformLUT.at(row);
}


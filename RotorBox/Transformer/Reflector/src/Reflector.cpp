
#include "Reflector.hpp"
#include <iostream>
#include <stdexcept>
#include <toml.hpp>

Reflector::Reflector(std::string fileName) {
    type = TransformerType::Reflector;
    initTransformLUT(fileName);
}

/**
 * @details Reflectors are symmetric components.
 * This function orchestrates the initialization by delegating TOML parsing
 * to parseConfig and then initializing the reverse mapping.
 */
void Reflector::initTransformLUT(std::string fileName) {
    parseConfig(fileName);

    // Initialize reverse transformation vector to -1.
    fillTransformRow(1, -1);
}

/**
 * @details Parses the reflector's wiring map from a TOML file.
 * @internal This method enforces strict size and type checking to ensure
 * the configuration matches the expected transformer size and type.
 */
void Reflector::parseConfig(std::string fileName) {
    toml::value data;
    parseBasicConfig(fileName, "reflector", data);

    try {
        auto arr = toml::find<std::vector<int>>(data, "reflector", "map");
        if (arr.size() != TRANSFORMER_SIZE) {
            throw std::runtime_error("TOML array size mismatch");
        }

        for (size_t i = 0; i < arr.size(); ++i) {
            setTransformValue(0, i, arr[i]);
        }

    } catch (const std::exception& e) {
        throw std::runtime_error("TOML parse error: " + std::string(e.what()));
    }
}

/**
 * @details Performs the signal reflection.
 * @internal The 'reverse' parameter is ignored for reflectors as they
 * occupy the 'turn-around' point in the signal path.
 */
int Reflector::transform(int position, bool reverse) {
    int newPosition = getTransformValue((int)reverse, position);
    // transformLUT[reverse][position] = -1,  when reverse is true
    return newPosition;
}

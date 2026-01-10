
#include "Reflector.hpp"
#include <iostream>
#include <toml.hpp>

Reflector::Reflector(std::string fileName){
    type = TransformerType::Reflector;
    initTransformLUT(fileName);
}

Reflector::~Reflector(){}

/**
 * @details Reflectors are symmetric components. 
 * This function orchestrates the initialization by delegating TOML parsing 
 * to parseConfig and then initializing the reverse mapping.
 */
bool Reflector::initTransformLUT(std::string fileName){
    if (!parseConfig(fileName)) {
        return false;
    }

    // Initialize reverse transformation vector to -1.
    fillTransformRow(1, -1);

    return true;
}

/**
 * @details Parses the reflector's wiring map from a TOML file.
 * @internal This method enforces strict size and type checking to ensure 
 * the configuration matches the expected transformer size and type.
 */
bool Reflector::parseConfig(std::string fileName){
    toml::value data;
    if (!parseBasicConfig(fileName, "reflector", data)) {
        return false;
    }

    try {
        auto arr = toml::find<std::vector<int>>(data, "reflector", "map");
        if(arr.size() != TRANSFORMER_SIZE) {
            std::cerr << "TOML array size mismatch" << std::endl;
            return false;
        }

        for(size_t i = 0; i < arr.size(); ++i) {
            setTransformValue(0, i, arr[i]);
        }

        return true;

    } catch(const std::exception& e) {
        std::cerr << "TOML parse error: " << e.what() << std::endl;
        return false;
    }
}


/**
 * @details Performs the signal reflection.
 * @internal The 'reverse' parameter is ignored for reflectors as they 
 * occupy the 'turn-around' point in the signal path.
 */
int Reflector::transform(int position, bool reverse){
    int newPosition = getTransformValue((int)reverse, position);
    // transformLUT[reverse][position] = -1,  when reverse is true 
    return newPosition;
}

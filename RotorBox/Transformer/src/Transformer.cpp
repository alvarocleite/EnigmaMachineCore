#include "../include/Transformer.hpp"

#include <fstream>
#include <iostream>
#include <string>

#include <toml.hpp> 

/**
 * @brief Constructor for the Transformer class.
 * Initializes the transformer type to notDefined.
 */
Transformer::Transformer(){
    type = notDefined;
}

Transformer::~Transformer(){}

/** 
 * @brief Calculates the size of the transformation lookup table (LUT).
 * 
 * @return int Returns the size of the transformation lookup table.
 */
int Transformer::sizeOfTransformLUT(){
    return sizeof(transformLUT) / sizeof(transformLUT[0][0]);
}

/** 
 * @brief Returns the type of the transformer.
 * 
 * @return transformerType Returns the type of the transformer (rotor, reflector, or notDefined).
*/
transformerType Transformer::getType(){
    return type;
}

/**
 * @brief Initializes the forward transformation lookup table from a TOML file.
 * For rotors, it also initializes the notch position. For reflectors, it sets the notch position to TRANSFORMER_SIZE.
 * 
 * @param fileName The name of the file containing the transformation data.
 * @return int Returns the notch position on success, or an "-1" on failure.
 */
int Transformer::initForwardTransformLUT(std::string fileName){
    int notchPosition = 0;
    try {
        auto data = toml::parse(fileName);
        auto type = toml::find<std::string>(data, "type");

        // Check the size of the transformer
        auto size = toml::find<int>(data, "size");
        if (size != TRANSFORMER_SIZE) {
            std::cerr << "Transformer size mismatch: expected " << TRANSFORMER_SIZE << ", got " << size << std::endl;
            return -1; // Indicating failure
        }

        // In case of rotor config file
        if (type == "rotor") {
            if (this->type != rotor) {
                std::cerr << "Wrong config file" << std::endl;
                return -1; // Indicating failure
            }
            
            // Init notch position
            auto notch = toml::find<int>(data, "rotor", "notchPosition");
            notchPosition = notch;
            
            auto arr = toml::find<std::vector<int>>(data, "rotor", "forward");

            // Check arr size
            if(arr.size() != TRANSFORMER_SIZE) {
                std::cerr << "TOML array size mismatch" << std::endl;
                return -1; // Indicating failure
            }

            // Initialize forward transformation vector
            for(size_t i = 0; i < arr.size(); ++i) {
                transformLUT[0][i] = arr[i];
            }
        }

        // In case of reflector config file
        else if (type == "reflector") {
            if (this->type != reflector) {
                std::cerr << "Wrong config file" << std::endl;
                return -1; // Indicating failure
            }

            auto arr = toml::find<std::vector<int>>(data, "reflector", "map");

            // Check arr size
            if(arr.size() != TRANSFORMER_SIZE) {
                std::cerr << "TOML array size mismatch" << std::endl;
                return -1; // Indicating failure
            }

            // Initialize forward transformation vector
            for(size_t i = 0; i < arr.size(); ++i) {
                transformLUT[0][i] = arr[i];
            }

            notchPosition = TRANSFORMER_SIZE; // Reflectors do not have a notch position, so it is set to TRANSFORMER_SIZE
        }
        
        // In case of unknown type
        else {
            std::cerr << "Unknown transformer type: " << type << std::endl;
            return -1; // Indicating failure
        }

    } 
    
    catch(const std::exception& e) {
        std::cerr << "TOML parse error: " << e.what() << std::endl;
        return -1; // Indicating failure
    }

    return notchPosition; // Return the notch position for rotors, or TRANSFORMER_SIZE for reflectors
}

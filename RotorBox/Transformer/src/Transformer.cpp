#include "Transformer.hpp"

#include <fstream>
#include <iostream>
#include <string>

#include <toml.hpp> 

Transformer::Transformer(){
    type = TransformerType::NotDefined;
}

Transformer::~Transformer(){}

int Transformer::sizeOfTransformLUT(){
    return transformLUT.size() * transformLUT[0].size();
}

TransformerType Transformer::getType(){
    return type;
}

/**
 * @details Parses the component's wiring from a TOML file.
 * The logic differentiates between "rotor" and "reflector" types to ensure 
 * correct array mapping and notch initialization.
 * 
 * @internal This method enforces strict size checking against TRANSFORMER_SIZE.
 * It uses polymorphic checks to ensure a rotor config isn't loaded into a reflector object.
 */
int Transformer::initForwardTransformLUT(std::string fileName){
    int notchPosition = 0;
    try {
        auto data = toml::parse(fileName);
        auto typeStr = toml::find<std::string>(data, "type");

        auto size = toml::find<int>(data, "size");
        if (size != TRANSFORMER_SIZE) {
            std::cerr << "Transformer size mismatch: expected " << TRANSFORMER_SIZE << ", got " << size << std::endl;
            return -1;
        }

        if (typeStr == "rotor") {
            if (this->type != TransformerType::Rotor) {
                std::cerr << "Wrong config file: expected rotor" << std::endl;
                return -1;
            }
            
            auto notch = toml::find<int>(data, "rotor", "notchPosition");
            notchPosition = notch;
            
            auto arr = toml::find<std::vector<int>>(data, "rotor", "forward");

            if(arr.size() != TRANSFORMER_SIZE) {
                std::cerr << "TOML array size mismatch" << std::endl;
                return -1;
            }

            for(size_t i = 0; i < arr.size(); ++i) {
                transformLUT.at(0).at(i) = arr[i];
            }
        }
        else if (typeStr == "reflector") {
            if (this->type != TransformerType::Reflector) {
                std::cerr << "Wrong config file: expected reflector" << std::endl;
                return -1;
            }

            auto arr = toml::find<std::vector<int>>(data, "reflector", "map");

            if(arr.size() != TRANSFORMER_SIZE) {
                std::cerr << "TOML array size mismatch" << std::endl;
                return -1;
            }

            for(size_t i = 0; i < arr.size(); ++i) {
                transformLUT.at(0).at(i) = arr[i];
            }

            notchPosition = TRANSFORMER_SIZE; 
        }
        else {
            std::cerr << "Unknown transformer type: " << typeStr << std::endl;
            return -1;
        }

    } 
    catch(const std::exception& e) {
        std::cerr << "TOML parse error: " << e.what() << std::endl;
        return -1;
    }

    return notchPosition; 
}

#include <iostream>
#include <string>
#include <stdexcept>

#include "RotorBox.hpp"
#include "Reflector.hpp"
#include "Rotor.hpp"

/**
 * @details Initializes a standard 3-rotor configuration with default assets.
 * All rotors are set to their initial '0' position.
 */
RotorBox::RotorBox(){
    nRotorCount = 3;
    for(int i = 0; i < nRotorCount; i++){
        rotorPositions.push_back(0);
    }
    
    // Will throw if initialization fails
    initTransformerVec(nRotorCount, std::vector<std::string> {assetsDir + "Rotor1.toml", 
                                         assetsDir + "Rotor2.toml", 
                                         assetsDir + "Rotor3.toml", 
                                         assetsDir + "Reflector.toml"});
}

/**
 * @details Initializes a custom rotor configuration.
 * Validates that the number of provided positions matches the rotor count before 
 * proceeding with transformer initialization and position setting.
 */
RotorBox::RotorBox(int nRotorCount, const std::vector<int> &rotorPositions, const std::vector<std::string> &rotorFiles){
    if (nRotorCount != (int)rotorPositions.size()){
        throw std::invalid_argument("Error: Number of rotors and number of rotor positions do not match.");
    }
    
    this->nRotorCount = nRotorCount;
    for(auto &position : rotorPositions){
        this->rotorPositions.push_back(position);
    }
    
    // Will throw if initialization fails
    initTransformerVec(nRotorCount, rotorFiles);

    for(int i = 0; i < nRotorCount; i++){
        transformerVec.at(i)->setPosition(this->rotorPositions.at(i));
    }
}

/**
 * @details Populates the internal transformer vector with unique pointers to Rotor and Reflector objects.
 * 
 * @internal This method enforces the architectural constraint that the transformer vector 
 * must contain exactly nRotorCount rotors followed by one reflector at the end. 
 * Memory is managed via std::unique_ptr to ensure proper cleanup.
 */
void RotorBox::initTransformerVec(int nRotorCount, const std::vector<std::string> &rotorFiles){
    transformerVec.clear();
    transformerVec.reserve(nRotorCount + 1);

    // Validate input size: n Rotors + 1 Reflector
    if (rotorFiles.size() < (size_t)(nRotorCount + 1)) {
        throw std::runtime_error("Error: Insufficient configuration files for rotors and reflector.");
    }

    int index = 0;
    while(index < nRotorCount){
        transformerVec.push_back(std::make_unique<Rotor>(rotorFiles[index]));
        index++;
    };
    transformerVec.push_back(std::make_unique<Reflector>(rotorFiles[nRotorCount]));
}

void RotorBox::printTransformerVec(){
    for(auto &transformer : transformerVec){
        std::cout << "Transformer Type: " << static_cast<int>(transformer->getType()) << std::endl;
    }
}

/**
 * @details The signal path inside the RotorBox simulates the actual wiring:
 * 1. Mechanical Step: Rotate rotors BEFORE signal processing.
 * 2. Forward Pass: Right-to-Left from the entry wheel through all rotors.
 * 3. Reflector: Swaps the character and sends it back.
 * 4. Reverse Pass: Left-to-Right back through the rotors using inverse mappings.
 */
int RotorBox::keyTransform(int input){
    updateRotors();

    // transform through rotors forward
    bool reverse = false;
    int newPosition = input;
    for(int i = 0; i < nRotorCount; i++){
        newPosition = transformerVec.at(i)->transform(newPosition, reverse);
    }

    // reflector
    newPosition = transformerVec.at(nRotorCount)->transform(newPosition, reverse);

    // transform through rotors in reverse
    reverse = true;
    for(int i = nRotorCount - 1; i >= 0; i--){ 
        newPosition = transformerVec.at(i)->transform(newPosition, reverse);
    }

    return newPosition;
}

/**
 * @details Implements odometer-style stepping logic.
 * The right-most rotor (index 0) always rotates. 
 * Subsequent rotors rotate only if the preceding rotor hits its notch.
 * 
 * @internal This is a simplified linear stepping. Real Enigma "double stepping" 
 * is not implemented here to favor modularity over exact historical fidelity in this version.
 */
void RotorBox::updateRotors(){
    int rotorIx = 0;
    int isNotch = 0;
    
    do{
        isNotch = transformerVec.at(rotorIx)->rotate();
        rotorIx++;
    } while (rotorIx < nRotorCount && isNotch == 1);
}

#include <iostream>
#include <string>

#include "RotorBox.hpp"
#include "Reflector.hpp"
#include "Rotor.hpp"

RotorBox::RotorBox(){
    nRotorCount = 3;
    for(int i = 0; i < nRotorCount; i++){
        rotorPositions.push_back(0);
    }
    if (initTransformerVec(nRotorCount, std::vector<std::string> {assetsDir + "Rotor1.toml", 
                                         assetsDir + "Rotor2.toml", 
                                         assetsDir + "Rotor3.toml", 
                                         assetsDir + "Reflector.toml"}) != 0) {
        std::cerr << "Error: Failed to initialize default transformers in RotorBox." << std::endl;
    }
}

RotorBox::RotorBox(int nRotorCount, const std::vector<int> &rotorPositions, const std::vector<std::string> &rotorFiles){
    if (nRotorCount != rotorPositions.size()){
        std::cerr << "Error: Number of rotors and number of rotor positions do not match." << std::endl;
        return;
    }
    
    this->nRotorCount = nRotorCount;
    for(auto &position : rotorPositions){
        this->rotorPositions.push_back(position);
    }
    
    if (initTransformerVec(nRotorCount, rotorFiles) != 0) {
        std::cerr << "Error: Failed to initialize transformers in RotorBox." << std::endl;
        return;
    }

    for(int i = 0; i < nRotorCount; i++){
        transformerVec[i]->setPosition(this->rotorPositions[i]);
    }
}

RotorBox::~RotorBox(){}

int RotorBox::initTransformerVec(int nRotorCount, const std::vector<std::string> &rotorFiles){
    int index = 0;
    while(index < nRotorCount){
        transformerVec.push_back(std::make_unique<Rotor>(rotorFiles[index]));
        index++;
    };
    transformerVec.push_back(std::make_unique<Reflector>(rotorFiles[nRotorCount]));

    return 0;
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
        newPosition = transformerVec[i]->transform(newPosition, reverse);
    }

    // reflector
    newPosition = transformerVec[nRotorCount]->transform(newPosition, reverse);

    // transform through rotors in reverse
    reverse = true;
    for(int i = nRotorCount - 1; i >= 0; i--){ 
        newPosition = transformerVec[i]->transform(newPosition, reverse);
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
int RotorBox::updateRotors(){
    int rotorIx = 0;
    int isNotch = 0;
    
    do{
        isNotch = transformerVec[rotorIx]->rotate();
        rotorIx++;
    } while (rotorIx < nRotorCount && isNotch == 1);

    return 0;
}

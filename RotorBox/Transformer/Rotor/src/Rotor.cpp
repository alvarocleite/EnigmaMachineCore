#include "Rotor.hpp"

#include <algorithm>
#include <string>

Rotor::Rotor(std::string fileName){
    notchPosition = 0;
    rotorRotationCount = 0;
    type = TransformerType::Rotor;
    initTransformLUT(fileName);
    initRotorPosition();
}

Rotor::~Rotor(){}

/**
 * @details Initializes the rotor's wiring configuration.
 * This function orchestrates the initialization process. It first loads the 
 * forward transformation table and notch position from the configuration file, 
 * and then triggers the generation of the reverse transformation table.
 */
bool Rotor::initTransformLUT(std::string fileName){
    bool canBeInitialized = true;
    int notchPosition = 0;

    notchPosition = initForwardTransformLUT(fileName);
    
    if (notchPosition > -1 && notchPosition < TRANSFORMER_SIZE){
        this->notchPosition = notchPosition;
    } else { 
        return false; 
    }

    canBeInitialized = initReverseTransformLUT();

    return canBeInitialized;
}

/**
 * @details Generates the mathematical inverse of the forward wiring to handle the return signal.
 * @internal If Forward(X) = Y, then Reverse(Y) = X. This ensures the signal can traverse 
 * back through the rotors after being reflected. It iterates through the forward LUT 
 * to map outputs back to inputs.
 */
bool Rotor::initReverseTransformLUT(){
    bool canBeInitialized = true;
    const auto& forwardRow = transformLUT.at(0); 
    auto& reverseRow       = transformLUT.at(1);

    for (int forwardValue = 0; forwardValue < TRANSFORMER_SIZE; forwardValue++){
        auto it = std::find_if(forwardRow.begin(), forwardRow.end(), 
            [forwardValue] (int value){ return value == forwardValue; }
        );

        if (it != forwardRow.end()){
            reverseRow.at(forwardValue) = std::distance(forwardRow.begin(), it);
        } else {
            reverseRow.at(forwardValue) = -1;
            canBeInitialized = false;
        }
    }

    return canBeInitialized;
}

int Rotor::initRotorPosition(int offset){
    rotorRotationCount = offset;
    return 0;
}

inline bool Rotor::isNotchPosition(int position){
    return (position == notchPosition);
}

/**
 * @details Handles the relative coordinate shift caused by the rotor's rotation.
 * 1. Shift input relative to rotor position.
 * 2. Apply internal wiring permutation.
 * 3. Shift output back to absolute machine coordinate system.
 * 
 * @internal This logic ensures that the physical rotation of the rotor 
 * effectively changes the entry and exit pins of the signal.
 */
int Rotor::transform(int position, bool reverse){
    position = (position + rotorRotationCount) % TRANSFORMER_SIZE; 
    position = transformLUT.at((int)reverse).at(position);
    position = (position - rotorRotationCount + TRANSFORMER_SIZE) % TRANSFORMER_SIZE;

    return position;
}

int Rotor::rotate(){
    rotorRotationCount = (rotorRotationCount + 1) % TRANSFORMER_SIZE;
    return isNotchPosition(rotorRotationCount) ? 1 : 0;
}

void Rotor::setPosition(int position){
    rotorRotationCount = position;
}

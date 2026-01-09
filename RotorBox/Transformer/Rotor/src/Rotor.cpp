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
 * @details Generates the mathematical inverse of the forward wiring to handle the return signal.
 * @internal If Forward(X) = Y, then Reverse(Y) = X. This allows the signal to pass 
 * back through the rotors after hitting the reflector.
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

    for (int fv = 0; fv < TRANSFORMER_SIZE; fv++){
        auto it = std::find_if(&transformLUT[0][0], &transformLUT[0][TRANSFORMER_SIZE-1], 
        [fv] (int value){
            return value == fv;
        });

        if (it != &transformLUT[1][TRANSFORMER_SIZE-1] + TRANSFORMER_SIZE){
            transformLUT[1][fv] = it - &transformLUT[0][0];
        } else {
            transformLUT[1][fv] = -1;
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
    position = transformLUT[(int)reverse][position];
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

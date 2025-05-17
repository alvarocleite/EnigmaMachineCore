#include "../include/Rotor.hpp"

#include <algorithm>

Rotor::Rotor(){
    notchPosition = 0;
    rotorRotationCount = 0;
    type = rotor;
    initTransformLUT();
    initRotorPosition();
}

Rotor::~Rotor(){}

bool Rotor::initTransformLUT(){
    bool canBeInitialized = true;
    
    // Initialize forward transformation vector
    for (int i = 0; i < TRANSFORMER_SIZE; i++){
        transformLUT[0][i] = (i + weightTransformVec[i]) % TRANSFORMER_SIZE;
    }

    // size of reverse transformation vector 
    int size = sizeOfTransformLUT()/2;
    
    for (int fv = 0; fv < TRANSFORMER_SIZE; fv++){
        // Find the target in the transformation vector
        auto it = std::find_if(&transformLUT[0][0], &transformLUT[0][TRANSFORMER_SIZE-1], 
        [fv] (int value){
            return value == fv;
        });

        // If the target is found, set the reverse transformation vector
        if (it != &transformLUT[1][TRANSFORMER_SIZE-1] + size){
            transformLUT[1][fv] = it - &transformLUT[0][0];
        } else {
            transformLUT[1][fv] = -1; // Not found
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

int Rotor::transform(int position, bool reverse){
    int newPosition;

    position += rotorRotationCount; 
    newPosition = transformLUT[(int)reverse][position];
    newPosition -= rotorRotationCount;

    return newPosition;
}

int Rotor::rotate(){
    rotorRotationCount = (rotorRotationCount + 1) % TRANSFORMER_SIZE;
    return isNotchPosition(rotorRotationCount) ? 1 : 0;
}

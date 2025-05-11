#include "../include/Rotor.hpp"

#include <algorithm>

Rotor::Rotor(){
    notchPosition = 0;
    type = rotor;
    initTransformLUT();
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
        auto it = std::find_if(&transformLUT[1][0], &transformLUT[1][TRANSFORMER_SIZE], 
        [fv] (int value){
            return value == fv;
        });

        // If the target is found, set the reverse transformation vector
        if (it != &transformLUT[1][TRANSFORMER_SIZE] + size){
            transformLUT[1][fv] = *it;
        } else {
            transformLUT[1][fv] = -1; // Not found
            canBeInitialized = false;
        }
    }

    return canBeInitialized;

}

int Rotor::transform(int position, int &newPosition, bool reverse){
    int returnValue = 0;
    newPosition = transformLUT[reverse][position];

    return returnValue;
}

#include "../include/Rotor.hpp"

#include <algorithm>
#include <string>

Rotor::Rotor(std::string fileName){
    notchPosition = 0;
    rotorRotationCount = 0;
    type = rotor;
    initTransformLUT(fileName);
    initRotorPosition();
}

Rotor::~Rotor(){}

/** 
 * @brief Initializes the Rotor transformation lookup tables (LUT).
 * This function calls the function that reads the forward transformation data from a file and initializes it,
 * then it initializes the reverse transformation vector based on the forward transformation data.
 * 
 * @param fileName The name of the file containing the transformation data.
 * @return bool Returns true if successful, false otherwise.
 */
bool Rotor::initTransformLUT(std::string fileName){
    bool canBeInitialized = true;
    int notchPosition = 0;
    
    // Initialize forward transformation vector
    notchPosition == initForwardTransformLUT(fileName);

    // Check if the notch position is valid
    if (notchPosition > -1 && notchPosition < TRANSFORMER_SIZE){
        this->notchPosition = notchPosition;
    } else { 
        return false; // Initialization failed
    }

    // Initialize reverse transformation vector 
    for (int fv = 0; fv < TRANSFORMER_SIZE; fv++){
        // Find the target in the transformation vector
        auto it = std::find_if(&transformLUT[0][0], &transformLUT[0][TRANSFORMER_SIZE-1], 
        [fv] (int value){
            return value == fv;
        });

        // If the target is found, set the reverse transformation vector
        if (it != &transformLUT[1][TRANSFORMER_SIZE-1] + TRANSFORMER_SIZE){
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
    position = (position + rotorRotationCount) % TRANSFORMER_SIZE; 
    position = transformLUT[(int)reverse][position];
    position = (position - rotorRotationCount + TRANSFORMER_SIZE) % TRANSFORMER_SIZE;

    return position;
}

int Rotor::rotate(){
    rotorRotationCount = (rotorRotationCount + 1) % TRANSFORMER_SIZE;
    return isNotchPosition(rotorRotationCount) ? 1 : 0;
}

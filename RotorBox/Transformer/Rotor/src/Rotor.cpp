#include "../include/Rotor.hpp"

#include <algorithm>
#include <string>

/**
 * @brief Constructor for the Rotor class.
 * Sets the notch position to 0 and initializes the rotor rotation count.
 * Initializes the rotor with a transformation lookup table (LUT) from a file.
 */
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

/**
 * @brief Initializes the rotor position.
 * This function sets the initial position of the rotor based on the provided offset.
 * 
 * @param offset The initial offset for the rotor position.
 * @return int Returns 0 on success.
 */
int Rotor::initRotorPosition(int offset){
    rotorRotationCount = offset;
    return 0;
}

/**
 * @brief Checks if the given position is the notch position.
 * 
 * @param position The position to check.
 * @return true if the position is the notch position, false otherwise.
 */
inline bool Rotor::isNotchPosition(int position){
    return (position == notchPosition);
}

/**
 * @brief Transforms the given position based on the transformation lookup table (LUT).
 * 
 * @param position The current position in the rotor.
 * @param reverse If true, transforms using the reverse LUT; otherwise, uses the forward LUT.
 * @return int The transformed position.
 */
int Rotor::transform(int position, bool reverse){
    position = (position + rotorRotationCount) % TRANSFORMER_SIZE; 
    position = transformLUT[(int)reverse][position];
    position = (position - rotorRotationCount + TRANSFORMER_SIZE) % TRANSFORMER_SIZE;

    return position;
}

/**
 * @brief Rotates the rotor by one position.
 * If the rotor reaches the notch position, it returns 1, indicating that the next rotor should rotate.
 * Otherwise, it returns 0.
 * 
 * @return int Returns 1 if the rotor reached the notch position, otherwise returns 0.
 */
int Rotor::rotate(){
    rotorRotationCount = (rotorRotationCount + 1) % TRANSFORMER_SIZE;
    return isNotchPosition(rotorRotationCount) ? 1 : 0;
}

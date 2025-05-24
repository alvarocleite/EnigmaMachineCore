
#include "../include/Reflector.hpp"

Reflector::Reflector(std::string fileName){
    type = reflector;
    initTransformLUT(fileName);
}

Reflector::~Reflector(){}

/** 
 * @brief Initializes the transformation lookup table (LUT) for the reflector.
 * This function the function that reads the transformation data from a file and initializes 
 * the forward vector. 
 * This function also initializes the reverse transformation vector with "-1".
 * 
 * @param fileName The name of the file containing the transformation data.
 * @return bool Returns true if the initialization is successful, false otherwise.
 */
bool Reflector::initTransformLUT(std::string fileName){
    int notchPosition = 0;

    // Initialize forward transformation vector
    notchPosition = initForwardTransformLUT(fileName);

    if (notchPosition != TRANSFORMER_SIZE){
        return false; // Initialization failed
    }

    // Initialize reverse transformation vector
    for (int i = 0; i < TRANSFORMER_SIZE; i++){
        transformLUT[1][i] = -1;
    }

    return true;
}

int Reflector::transform(int position, bool reverse){
    int newPosition = transformLUT[reverse][position];
    // transformLUT[reverse][position] = -1,  when reverse is true 
    return newPosition;
}

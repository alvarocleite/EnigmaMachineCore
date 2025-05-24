
#include "../include/Reflector.hpp"

/**
 * @brief Constructor for the Reflector class.
 * Initializes the reflector with a transformation lookup table (LUT) from a file.
 * Sets the type of the transformer to reflector.
 */
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

/**
 * @brief Transforms the given position using the reflector's transformation lookup table (LUT).
 * 
 * @param position The input position to be transformed.
 * @param reverse It should not be set to true.
 * @return int The transformed position. Returns "-1" if the position is not found in the LUT when reverse is true.
 */
int Reflector::transform(int position, bool reverse){
    int newPosition = transformLUT[reverse][position];
    // transformLUT[reverse][position] = -1,  when reverse is true 
    return newPosition;
}

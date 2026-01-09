
#include "Reflector.hpp"

Reflector::Reflector(std::string fileName){
    type = TransformerType::Reflector;
    initTransformLUT(fileName);
}

Reflector::~Reflector(){}

/**
 * @details Reflectors are symmetric components. 
 * While they only have one physical set of wirings, the system treats them as 
 * non-notching transformers with a static wiring map.
 */
bool Reflector::initTransformLUT(std::string fileName){
    int notchPosition = 0;

    notchPosition = initForwardTransformLUT(fileName);

    if (notchPosition != TRANSFORMER_SIZE){
        return false; 
    }

    // Initialize reverse transformation vector to -1.
    // Conceptually, for a reflector, Forward == Reverse, but the signal 
    // only ever passes through it once per key press.
    for (int i = 0; i < TRANSFORMER_SIZE; i++){
        transformLUT[1][i] = -1;
    }

    return true;
}

/**
 * @details Performs the signal reflection.
 * @internal The 'reverse' parameter is ignored for reflectors as they 
 * occupy the 'turn-around' point in the signal path.
 */
int Reflector::transform(int position, bool reverse){
    int newPosition = transformLUT[(int)reverse][position];
    // transformLUT[reverse][position] = -1,  when reverse is true 
    return newPosition;
}

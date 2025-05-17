
#include "../include/Reflector.hpp"

Reflector::Reflector(std::string fileName){
    type = reflector;
    initTransformLUT(fileName);
}

Reflector::~Reflector(){}

bool Reflector::initTransformLUT(std::string fileName){
    // Initialize forward transformation vector
    initForwardTransformLUT(fileName);

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

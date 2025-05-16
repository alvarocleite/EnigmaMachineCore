
#include "../include/Reflector.hpp"

Reflector::Reflector(){
    type = reflector;
    initTransformLUT();
}

Reflector::~Reflector(){}

bool Reflector::initTransformLUT(){
    // Initialize forward transformation vector
    for (int i = 0; i < TRANSFORMER_SIZE; i++){
        transformLUT[0][i] = (i + weightTransformVec[i]) % TRANSFORMER_SIZE;
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

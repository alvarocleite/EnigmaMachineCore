
#include "../include/Reflector.hpp"

Reflector::Reflector(){
    type = reflector;
    initTransformLUT();
}

Reflector::~Reflector(){}

void Reflector::initTransformLUT(){
    // Initialize forward transformation vector
    for (int i = 0; i < TRANSFORMER_SIZE; i++){
        transformLUT[0][i] = (i + weightTransformVec[i]) % TRANSFORMER_SIZE;
    }

    // Initialize reverse transformation vector
    for (int i = 0; i < TRANSFORMER_SIZE; i++){
        transformLUT[1][i] = -1;
    }
}

int Reflector::transform(int position, int &newPosition, bool reverse){
    newPosition = transformLUT[reverse][position];
    return reverse * -1;
}

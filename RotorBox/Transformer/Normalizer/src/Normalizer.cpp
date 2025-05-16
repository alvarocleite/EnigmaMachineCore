#include "../include/Normalizer.hpp"

Normalizer::Normalizer(){
    type = normalizer;
}

Normalizer::~Normalizer(){}

int Normalizer::transform(int position, int &newPosition, bool reverse){
    int returnValue = 0;

    newPosition = (position + (rRotationCount - lRotationCount) * (reverse ? -1 : 1)) % 26;

    return returnValue;
}

int Normalizer::setRotationCount(const int lRotation, const int rRotation){
    lRotationCount = lRotation;
    rRotationCount = rRotation;
    return 0;
};

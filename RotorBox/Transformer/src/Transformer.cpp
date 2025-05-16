#include "../include/Transformer.hpp"

Transformer::Transformer(){
    type = notDefined;
}

Transformer::~Transformer(){}

int Transformer::sizeOfTransformLUT(){
    return sizeof(transformLUT) / sizeof(transformLUT[0][0]);
}

transformerType Transformer::getType(){
    return type;
}


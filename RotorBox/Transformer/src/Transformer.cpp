#include "../include/Transformer.hpp"

Transformer::Transformer(){
    type = notDefined;
    initTransformLUT();
}

Transformer::~Transformer(){}

void Transformer::initTransformLUT(){
    for (int i = 0; i < TRANSFORMER_SIZE; i++){
        for (int j = 0; j < TRANSFORMER_SIZE; j++){
            transformLUT[i][j] = 0;
        }
    }
}

int Transformer::sizeOfTransformLUT(){
    return sizeof(transformLUT) / sizeof(transformLUT[0][0]);
}

transformerType Transformer::getType(){
    return type;
}


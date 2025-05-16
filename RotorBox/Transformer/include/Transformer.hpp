#pragma once

#define TRANSFORMER_SIZE 26

typedef enum transformerType{
    notDefined = 0,
    normalizer,
    rotor,
    reflector
} transformerType;

class Transformer{
protected:
    transformerType type;
    int weightTransformVec[TRANSFORMER_SIZE] = {0};
    int transformLUT[2][TRANSFORMER_SIZE] = {0};
    virtual bool initTransformLUT() = 0;
public:
    Transformer();
    ~Transformer();

    virtual int transform(int position, int &newPosition, bool reverse = false) = 0;
    int sizeOfTransformLUT();
    transformerType getType();
};

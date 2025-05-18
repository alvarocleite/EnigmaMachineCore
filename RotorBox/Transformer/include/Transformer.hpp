#pragma once

#include <string>

#include "../../config/config.hpp"

typedef enum transformerType{
    notDefined = 0,
    rotor,
    reflector
} transformerType;

class Transformer{
protected:
    transformerType type;
    int transformLUT[2][TRANSFORMER_SIZE] = {0};
    virtual bool initTransformLUT(std::string fileName) = 0;
    int initForwardTransformLUT(std::string fileName);
public:
    Transformer();
    ~Transformer();

    virtual int transform(int position, bool reverse = false) = 0;
    virtual int rotate() = 0;

    int sizeOfTransformLUT();
    transformerType getType();
};

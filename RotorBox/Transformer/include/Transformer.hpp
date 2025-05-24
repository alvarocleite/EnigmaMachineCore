#pragma once

#include <string>

#include "../../config/config.hpp"

/** 
 * @brief Enum representing the type of transformer.
 */
typedef enum transformerType{
    notDefined = 0,
    rotor,
    reflector
} transformerType;

/** 
 * @brief Base class for transformers (rotors and reflectors).
 * This class provides the interface for transforming positions and initializing transformation lookup tables (LUT).
 */
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

#pragma once

#include <string>

#include "config.hpp"

/** 
 * @brief Enum representing the type of transformer.
 */
enum class TransformerType {
    NotDefined = 0,
    Rotor,
    Reflector
};

/** 
 * @brief Base class for transformers (rotors and reflectors).
 * This class provides the interface for transforming positions and initializing transformation lookup tables (LUT).
 */
class Transformer{
protected:
    TransformerType type;
    int transformLUT[2][TRANSFORMER_SIZE] = {0};
    virtual bool initTransformLUT(std::string fileName) = 0;
    int initForwardTransformLUT(std::string fileName);
public:
    Transformer();
    virtual ~Transformer();

    virtual int transform(int position, bool reverse = false) = 0;
    virtual int rotate() = 0;
    virtual void setPosition(int position) {};

    int sizeOfTransformLUT();
    TransformerType getType();
};

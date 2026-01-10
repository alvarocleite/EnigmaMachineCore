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

    /**
     * @brief Initializes the forward transformation lookup table from a TOML file.
     * 
     * @param fileName The name of the file containing the transformation data.
     * @return int Returns the notch position on success, or -1 on failure.
     */
    int initForwardTransformLUT(std::string fileName);
public:
    /**
     * @brief Constructor for the Transformer class.
     * Initializes the transformer type to NotDefined.
     */
    Transformer();
    virtual ~Transformer();

    virtual int transform(int position, bool reverse = false) = 0;
    virtual int rotate() = 0;
    virtual void setPosition(int position) {};

    /** 
     * @brief Calculates the size of the transformation lookup table (LUT).
     * 
     * @return int Returns the size of the transformation lookup table.
     */
    int sizeOfTransformLUT();

    /** 
     * @brief Returns the type of the transformer.
     * 
     * @return TransformerType Returns the type of the transformer (Rotor, Reflector, or NotDefined).
    */
    TransformerType getType();
};

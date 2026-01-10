#pragma once

#include <string>
#include <array>
#include <toml.hpp>

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
private:
    std::array<std::array<int, TRANSFORMER_SIZE>, 2> transformLUT;

protected:
    TransformerType type;
    
    /** 
     * @brief Pure virtual function to initialize the transformation lookup table (LUT).
     * This must be implemented by derived classes to handle specific configuration loading.
     * 
     * @param fileName The path to the configuration file.
     * @return bool Returns true if initialization is successful, false otherwise.
     */
    virtual bool initTransformLUT(std::string fileName) = 0;

    /**
     * @brief Parses the common configuration from a TOML file (size and type checks).
     * 
     * @param fileName The path to the configuration file.
     * @param expectedType The expected type string ("rotor" or "reflector").
     * @param outData Reference to store the parsed TOML data if successful.
     * @return bool Returns true if parsing and checks pass, false otherwise.
     */
    bool parseBasicConfig(std::string fileName, std::string expectedType, toml::value& outData);

    /**
     * @brief Sets a value in the transformation lookup table.
     * 
     * @param row The row index (0 for forward, 1 for reverse).
     * @param col The column index (input position 0-25).
     * @param value The value to set (output position).
     */
    void setTransformValue(int row, int col, int value);

    /**
     * @brief Gets a value from the transformation lookup table.
     * 
     * @param row The row index (0 for forward, 1 for reverse).
     * @param col The column index (input position 0-25).
     * @return int The value at the specified position.
     */
    int getTransformValue(int row, int col) const;

    /**
     * @brief Fills a row of the transformation lookup table with a specific value.
     * 
     * @param row The row index to fill.
     * @param value The value to fill the row with.
     */
    void fillTransformRow(int row, int value);
    
    /**
     * @brief Gets a read-only reference to a row in the transformation lookup table.
     * Useful for using standard algorithms like std::find.
     * 
     * @param row The row index to retrieve.
     * @return const std::array<int, TRANSFORMER_SIZE>& Reference to the row array.
     */
    const std::array<int, TRANSFORMER_SIZE>& getTransformRow(int row) const;

public:
    /**
     * @brief Constructor for the Transformer class.
     * Initializes the transformer type to NotDefined.
     */
    Transformer();
    virtual ~Transformer();

    virtual int transform(int position, bool reverse = false) = 0;
    virtual int rotate() = 0;
    virtual void setPosition(int /*position*/) {};

    /** 
     * @brief Calculates the size of the transformation lookup table (LUT).
     * 
     * @return int Returns the size of the transformation lookup table.
     */
    int sizeOfTransformLUT() const;

    /** 
     * @brief Returns the type of the transformer.
     * 
     * @return TransformerType Returns the type of the transformer (Rotor, Reflector, or NotDefined).
    */
    TransformerType getType() const;
};

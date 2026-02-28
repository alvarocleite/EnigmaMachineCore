#pragma once

#include <array>
#include <string>
#include <toml.hpp>

#include "config.hpp"

/**
 * @file
 * @brief Header file for the Transformer class.
 */

/**
 * @brief Enum representing the type of transformer.
 */
enum class TransformerType { Undefined = 0, Rotor, Reflector };

/**
 * @brief Base class for transformers (rotors and reflectors).
 * This class provides the interface for transforming positions and initializing transformation lookup tables (LUT).
 */
class Transformer {
private:
    std::array<std::array<int, TRANSFORMER_SIZE>, 2> lookupTable;

protected:
    TransformerType type;

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
    virtual ~Transformer() = default;

    /**
     * @details this method is const-qualified, ensuring that the signal transformation does not modify the internal
     * state of the transformer.
     */
    virtual int transform(int position, bool reverse = false) const = 0;

    virtual int rotate() = 0;
    virtual void setPosition(int /*position*/) {};
    virtual int getPosition() const { return 0; };

    /**
     * @brief Calculates the size of the transformation lookup table (LUT).
     *
     * @return int Returns the size of the transformation lookup table.
     */
    int sizeOfLookupTable() const;

    /**
     * @brief Returns the type of the transformer.
     *
     * @return TransformerType Returns the type of the transformer (Rotor, Reflector, or NotDefined).
     */
    TransformerType getType() const;
};
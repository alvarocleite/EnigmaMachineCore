/**
 * @file EnigmaError.hpp
 * @brief Error codes and Result type for the EnigmaCore library.
 */

#pragma once

#include <nonstd/expected.hpp>

namespace enigma {

/**
 * @brief Error codes for Enigma operations.
 * Used as the error type in Result<T> to represent failure conditions
 * without requiring exceptions.
 */
enum class EnigmaError {
    None = 0,
    FileNotFound,
    ConfigFieldMissing,
    TransformerSizeMismatch,
    ConfigCountMismatch,
    PlugBoardExceedsMaximum,
    PlugBoardCountMismatch,
    RotorWiringOutOfRange,
    RotorWiringDuplicate,
    RotorWiringNotBijective,
    PlugBoardPortOutOfRange,
    PlugBoardPortConflict,
    RotorBoxPositionMismatch,
};

/**
 * @brief Result type alias for Enigma operations.
 * @tparam T The success value type.
 *
 * Usage:
 *   Result<int> result = someFunction();
 *   if (!result) { handleError(result.error()); }
 *   int value = *result;
 */
template <typename T>
using Result = nonstd::expected<T, EnigmaError>;

}  // namespace enigma

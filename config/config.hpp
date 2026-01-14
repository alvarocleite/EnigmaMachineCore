#pragma once
#include <string_view>

/**
 * @file config.hpp
 * @brief Global configuration constants for the Enigma machine.
 */

/** @brief Size of the alphabet/transformer (Standard Enigma is 26). */
inline constexpr int TRANSFORMER_SIZE = 26;

/** @brief Maximum number of allowed pairs on the plugboard. */
inline constexpr int PLUGBOARD_MAX_PAIRS = 10;

/** @brief Default assets base directory. */
inline constexpr std::string_view assetsDir = "assets/";
#pragma once
#include <string_view>

/**
 * @file config.hpp
 * @brief Global configuration constants for the Enigma machine.
 */

/** @brief Size of the alphabet/transformer (Standard Enigma is 26). */
inline constexpr int TRANSFORMER_SIZE = 26;

/** @brief Maximum number of allowed pairs on the plugboard. */
inline constexpr int PLUGBOARD_MAX_PAIRS = 13;

/** @brief Default assets base directory. */
inline constexpr std::string_view assetsDir = "assets/";

/** @brief Default configuration files. */
inline constexpr std::string_view defaultRotor1File = "Rotor1.toml";
inline constexpr std::string_view defaultRotor2File = "Rotor2.toml";
inline constexpr std::string_view defaultRotor3File = "Rotor3.toml";
inline constexpr std::string_view defaultReflectorFile = "Reflector.toml";

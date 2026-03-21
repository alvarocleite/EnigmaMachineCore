#pragma once

#include <cstdint>
#include <string_view>

/**
 * @file EnigmaConfig.hpp
 * @brief Compile-time constants for embedded/WASM targets without dynamic allocation.
 *
 * These constants define platform-independent limits for POD data structures.
 * They are used by EnigmaData.hpp for fixed-size array definitions.
 */

namespace enigma {

/** @brief Size of the alphabet/transformer (Standard Enigma is 26). */
inline constexpr int TRANSFORMER_SIZE = 26;

/** @brief Maximum number of rotors (supports historical + future models). */
inline constexpr int MAX_ROTORS = 10;

/** @brief Maximum plugboard pairs (26 ports / 2 = 13 maximum connections). */
inline constexpr int MAX_PLUGBOARD_PAIRS = 13;

/** @brief Default assets base directory. */
inline constexpr std::string_view assetsDir = "assets/";

/** @brief Default configuration files. */
inline constexpr std::string_view defaultRotor1File = "Rotor1.toml";
inline constexpr std::string_view defaultRotor2File = "Rotor2.toml";
inline constexpr std::string_view defaultRotor3File = "Rotor3.toml";
inline constexpr std::string_view defaultReflectorFile = "Reflector.toml";

}  // namespace enigma

#pragma once

#include <array>
#include <type_traits>

#include "EnigmaConfig.hpp"
#include "EnigmaTypes.hpp"

namespace enigma {

/**
 * @brief POD configuration for a single rotor.
 *
 * No constructors, no heap allocation, trivially copyable.
 */
struct RotorData {
    /** @brief Notch position that triggers the next rotor to step (0-25). */
    AlphabetIndex notchPosition = 0;
    /** @brief Internal wiring permutation array (forward direction). Size: TRANSFORMER_SIZE. */
    std::array<AlphabetIndex, TRANSFORMER_SIZE> wiring = {};
};

/**
 * @brief POD configuration for a reflector.
 *
 * No constructors, no heap allocation, trivially copyable.
 */
struct ReflectorData {
    /** @brief Internal wiring permutation array (reflection map). Size: TRANSFORMER_SIZE. */
    std::array<AlphabetIndex, TRANSFORMER_SIZE> wiring = {};
};

/**
 * @brief POD representation of a single plugboard connection pair.
 *
 * Field names align with runtime PlugBoardPair. Uses -1 to indicate unused/unconnected ports.
 */
struct PlugBoardPairData {
    /** @brief Source port index (0-25), or -1 if unused. */
    AlphabetIndex sourcePortIndex = -1;
    /** @brief Destination port index (0-25), or -1 if unused. */
    AlphabetIndex destinationPortIndex = -1;
};

/**
 * @brief POD configuration for the plugboard.
 *
 * Uses array of structs for better cache locality.
 */
struct PlugBoardData {
    /** @brief Array of plugboard connection pairs. Size: MAX_PLUGBOARD_PAIRS. */
    std::array<PlugBoardPairData, MAX_PLUGBOARD_PAIRS> pairs = {};
};

/**
 * @brief Complete POD configuration for the Enigma Machine.
 *
 * All members are primitive types - can be stack-allocated or globally initialized.
 * Suitable for embedded systems, WebAssembly, and serialization.
 */
struct EnigmaMachineData {
    /** @brief Number of active rotors (typically 3 for historical Enigma). */
    int rotorCount = 0;
    /** @brief Initial rotational positions of rotors. Size: MAX_ROTORS. */
    std::array<AlphabetIndex, MAX_ROTORS> rotorPositions = {};
    /** @brief Rotor configurations. Size: MAX_ROTORS. */
    std::array<RotorData, MAX_ROTORS> rotors = {};
    /** @brief Reflector configuration. */
    ReflectorData reflector = {};
    /** @brief Plugboard configuration. */
    PlugBoardData plugBoard = {};
};

static_assert(std::is_trivially_copyable_v<enigma::RotorData>,
              "RotorData must be trivially copyable for POD/embedded compatibility");
static_assert(std::is_trivially_copyable_v<enigma::ReflectorData>,
              "ReflectorData must be trivially copyable for POD/embedded compatibility");
static_assert(std::is_trivially_copyable_v<enigma::PlugBoardPairData>,
              "PlugBoardPairData must be trivially copyable for POD/embedded compatibility");
static_assert(std::is_trivially_copyable_v<enigma::PlugBoardData>,
              "PlugBoardData must be trivially copyable for POD/embedded compatibility");
static_assert(std::is_trivially_copyable_v<enigma::EnigmaMachineData>,
              "EnigmaMachineData must be trivially copyable for POD/embedded compatibility");

}  // namespace enigma

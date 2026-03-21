#pragma once

#include <array>
#include <string>
#include <vector>
#include "EnigmaConfig.hpp"
#include "EnigmaData.hpp"
#include "EnigmaTypes.hpp"
#include "PlugBoardPair.hpp"

/**
 * @brief Configuration structure for an individual Rotor.
 */
struct RotorConfig {
    RotorConfig();
    /** @brief The position (0 - (TRANSFORMER_SIZE - 1)) at which the rotor triggers the turnover of the next rotor. */
    AlphabetIndex notchPosition = 0;
    /** @brief The internal wiring permutation array (forward direction). Must be of size TRANSFORMER_SIZE. */
    std::array<AlphabetIndex, enigma::TRANSFORMER_SIZE> wiring;
};

/**
 * @brief Configuration structure for a Reflector.
 */
struct ReflectorConfig {
    ReflectorConfig();
    /** @brief The internal wiring permutation array (reflection map). Must be of size TRANSFORMER_SIZE. */
    std::array<AlphabetIndex, enigma::TRANSFORMER_SIZE> wiring;
};

/**
 * @brief Configuration Data Transfer Object (DTO) for the Enigma Machine.
 * Holds the raw configuration data required to initialize the machine.
 *
 * @note This is a pure data structure. Logic for loading this from files
 * resides in EnigmaConfigLoader.
 */
struct EnigmaMachineConfig {
    /** @brief Number of rotors in the machine. */
    int rotorCount = 0;
    /** @brief Initial rotational positions of the rotors. */
    std::vector<AlphabetIndex> rotorPositions;
    /** @brief Configuration for each rotor (in order). */
    std::vector<RotorConfig> rotors;
    /** @brief Configuration for the reflector. */
    ReflectorConfig reflector;
    /** @brief Plugboard connection pairs. */
    std::array<PlugBoardPair, enigma::MAX_PLUGBOARD_PAIRS> plugBoardPairs;

    /**
     * @brief Default constructor.
     * Initializes plugboard pairs to -1 (unused).
     */
    EnigmaMachineConfig();

    /**
     * @brief Converts this configuration to a POD DTO for embedded/WASM targets.
     * @return enigma::EnigmaMachineData POD structure.
     */
    enigma::EnigmaMachineData toData() const;
};

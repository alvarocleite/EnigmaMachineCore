#pragma once

#include <array>
#include <string>
#include <vector>
#include "PlugBoard.hpp"  // For Pair_t and PLUGBOARD_MAX_PAIRS
#include "config.hpp"     // For TRANSFORMER_SIZE

/**
 * @brief Configuration structure for an individual Rotor.
 */
struct RotorConfig {
    int notchPosition = 0;
    std::vector<int> wiring;
};

/**
 * @brief Configuration structure for a Reflector.
 */
struct ReflectorConfig {
    std::vector<int> wiring;
};

/**
 * @brief Configuration Data Transfer Object (DTO) for the Enigma Machine.
 * Holds the raw configuration data required to initialize the machine.
 * 
 * @note This is a pure data structure. Logic for loading this from files
 * resides in EnigmaConfigLoader.
 */
struct EnigmaMachineConfig {
    int rotorCount = 0;
    std::vector<int> rotorPositions;
    std::vector<RotorConfig> rotors;
    ReflectorConfig reflector;
    std::array<Pair_t, PLUGBOARD_MAX_PAIRS> plugBoardPairs;

    /**
     * @brief Default constructor.
     * Initializes plugboard pairs to -1 (unused).
     */
    EnigmaMachineConfig();
};
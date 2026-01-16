/**
 * @file
 * @brief Header file for the EnigmaMachine class.
 */

#pragma once

#include <string_view>
#include <tuple>
#include "EnigmaMachineConfig.hpp"
#include "PlugBoard.hpp"
#include "RotorBox.hpp"

/**
 * @brief Class representing the Enigma machine.
 * This class encapsulates the functionality of the Enigma machine, including the rotor box
 * and the transformation of input keys through the rotors and reflector.
 */
class EnigmaMachine {
private:
    RotorBox rotorBox;
    PlugBoard plugBoard;  // Optional: if you want to include a plugboard for additional transformations

public:
    /**
     * @brief Default Constructor for the EnigmaMachine class.
     *
     * Initializes a standard Enigma Machine configuration with:
     * - 3 Rotors (Rotor1, Rotor2, Rotor3) positioned at 0.
     * - A standard Reflector.
     * - An empty PlugBoard (identity mapping).
     *
     * This configuration is useful for basic testing or default behavior.
     * @throws std::runtime_error If default asset files cannot be found or parsed.
     */
    EnigmaMachine();

    /**
     * @brief Parameterized Constructor for the EnigmaMachine class (No PlugBoard).
     *
     * Initializes the machine with a custom RotorBox configuration and an empty PlugBoard.
     *
     * @param nRotorCount The number of rotors to be used in the RotorBox.
     * @param rotorPositions A vector defining the initial rotational position (0 - (TRANSFORMER_SIZE - 1)) for each
     * rotor.
     * @param transformerFiles A vector of file paths defining the wiring for each rotor and the reflector.
     *                   The last file in the list is expected to be the Reflector.
     * @throws std::invalid_argument If the number of transformer files does not match nRotorCount + 1.
     */
    EnigmaMachine(int nRotorCount, const std::vector<int>& rotorPositions,
                  const std::vector<std::string>& transformerFiles);

    /**
     * @brief Parameterized Constructor for the EnigmaMachine class (With PlugBoard).
     *
     * Initializes the machine with a fully custom configuration for both the RotorBox and PlugBoard.
     *
     * @param nRotorCount The number of rotors to be used in the RotorBox.
     * @param rotorPositions A vector defining the initial rotational position (0-(TRANSFORMER_SIZE - 1)) for each
     * rotor.
     * @param transformerFiles A vector of file paths defining the wiring for each rotor and the reflector.
     *                   The last file in the list is expected to be the Reflector.
     * @param plugBoardPairs An array of `Pair_t` defining the swaps to be configured on the PlugBoard.
     * @throws std::invalid_argument If the number of transformer files does not match nRotorCount + 1.
     */
    EnigmaMachine(int nRotorCount, const std::vector<int>& rotorPositions,
                  const std::vector<std::string>& transformerFiles,
                  const std::array<Pair_t, PLUGBOARD_MAX_PAIRS>& plugBoardPairs);

    /**
     * @brief Constructor using the configuration struct.
     *
     * @param config The parsed configuration object.
     */
    EnigmaMachine(const EnigmaMachineConfig& config);

    /**
     * @brief File-based Constructor for the EnigmaMachine class.
     *
     * Initializes the machine by parsing a TOML configuration file.
     *
     * This constructor delegates the parsing logic to `parseConfig` and then delegates
     * initialization to the main parameterized constructor.
     *
     * @param fileName The path to the TOML configuration file.
     * @param assetPath Optional base directory for assets (rotors/reflectors).
     * @throws std::runtime_error If the file cannot be parsed or contains invalid configuration data.
     */
    EnigmaMachine(std::string_view fileName, std::string_view assetPath = "");
    ~EnigmaMachine() = default;

    /**
     * @brief Transforms the input key through the rotor box.
     *
     * @param input The input key to be transformed.
     * @return int The transformed output key.
     */
    int keyTransform(int input);
};
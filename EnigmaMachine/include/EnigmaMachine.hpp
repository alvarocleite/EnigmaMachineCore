#pragma once

#include <tuple>
#include "RotorBox.hpp"
#include "PlugBoard.hpp"

/**
 * @brief Class representing the Enigma machine.
 * This class encapsulates the functionality of the Enigma machine, including the rotor box
 * and the transformation of input keys through the rotors and reflector.
 */
class EnigmaMachine{
private:
    RotorBox rotorBox;
    PlugBoard plugBoard; // Optional: if you want to include a plugboard for additional transformations

    /**
     * @brief Private Delegating Constructor.
     *
     * Used as an intermediary to unpack the configuration tuple returned by `parseConfig`
     * and pass the individual components to the main public constructor.
     *
     * @param config A tuple containing: {RotorCount, RotorPositions, RotorFiles, PlugBoardPairs}.
     */
    EnigmaMachine(std::tuple<int, std::vector<int>, std::vector<std::string>, std::array<Pair_t, PLUGBOARD_MAX_PAIRS>> config);

    /**
     * @brief Function used by fileName Constructor for the EnigmaMachine class.
     * Parses the configuration file to extract the number of rotors, their positions, files, and plugboard pairs.
     * 
     * @param fileName The name of the configuration file containing the settings for the Enigma machine.
     * @param assetPath Optional base directory to prepend to relative paths found in the config.
     * @return A tuple containing the number of rotors, their positions, files, and plugboard pairs.
     * @throws std::runtime_error if the configuration file is invalid or if the number of rotors, positions, and files do not match.
     */
    static std::tuple<int, std::vector<int>, std::vector<std::string>, std::array<Pair_t, PLUGBOARD_MAX_PAIRS>> parseConfig(const std::string& fileName, const std::string& assetPath = "");

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
     */
    EnigmaMachine();

    /**
     * @brief Parameterized Constructor for the EnigmaMachine class (No PlugBoard).
     *
     * Initializes the machine with a custom RotorBox configuration and an empty PlugBoard.
     *
     * @param nRotorCount The number of rotors to be used in the RotorBox.
     * @param rotorPositions A vector defining the initial rotational position (0-25) for each rotor.
     * @param rotorFiles A vector of file paths defining the wiring for each rotor and the reflector.
     *                   The last file in the list is expected to be the Reflector.
     */
    EnigmaMachine(int nRotorCount, const std::vector<int> &rotorPositions, const std::vector<std::string> &transformerFiles);

    /**
     * @brief Parameterized Constructor for the EnigmaMachine class (With PlugBoard).
     *
     * Initializes the machine with a fully custom configuration for both the RotorBox and PlugBoard.
     *
     * @param nRotorCount The number of rotors to be used in the RotorBox.
     * @param rotorPositions A vector defining the initial rotational position (0-25) for each rotor.
     * @param rotorFiles A vector of file paths defining the wiring for each rotor and the reflector.
     *                   The last file in the list is expected to be the Reflector.
     * @param plugBoardPairs An array of `Pair_t` defining the swaps to be configured on the PlugBoard.
     */
    EnigmaMachine(int nRotorCount, const std::vector<int> &rotorPositions, const std::vector<std::string> &transformerFiles, const std::array<Pair_t, PLUGBOARD_MAX_PAIRS> &plugBoardPairs);

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
    EnigmaMachine(std::string fileName, std::string assetPath = "");
    ~EnigmaMachine();

    /**
     * @brief Transforms the input key through the rotor box.
     * 
     * @param input The input key to be transformed.
     * @return int The transformed output key.
     */
    int keyTransform(int input);
};

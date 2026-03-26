/**
 * @file
 * @brief Header file for the RotorBox class.
 */

#pragma once

#include <memory>
#include <vector>

#include "EnigmaError.hpp"
#include "EnigmaMachineConfig.hpp"
#include "EnigmaTypes.hpp"
#include "IEnigmaObserver.hpp"
#include "Transformer.hpp"

/**
 * @brief Class representing a box of rotors in the Enigma machine.
 *
 * @image html RotorBox_Organization_Diagram.svg "RotorBox Organization" width=800px
 *
 * This class manages multiple rotors and a reflector, allowing for the transformation of input keys.
 * It handles the initialization of rotors, their positions, and the transformation process.
 */
class RotorBox {
private:
    int rotorCount;
    std::vector<AlphabetIndex> rotorPositions;
    std::vector<std::unique_ptr<Transformer>> transformers;
    std::vector<IEnigmaObserver*> observers;
    ILogger* logger;

    /**
     * @brief Initializes the transformer vector with rotors and a reflector.
     *
     * @param rotors A vector containing the configuration for each rotor.
     * @param reflector Configuration for the reflector.
     */
    void initTransformers(const std::vector<RotorConfig>& rotors, const ReflectorConfig& reflector);

    /**
     * @brief Updates the positions of the rotors.
     */
    void updateRotors();

public:
    /**
     * @brief Constructor for the RotorBox class.
     * Initializes the rotor box with a default number of rotors (3) and their positions (all set to 0).
     *
     * @param logger Optional logger for event reporting.
     * @throws std::runtime_error If initialization of transformers fails.
     */
    explicit RotorBox(ILogger* logger = nullptr);

    /**
     * @brief Constructor for the RotorBox class.
     * Initializes the rotor box with a specified number of rotors, their positions, and configurations.
     *
     * @param rotorPositions A vector containing the initial positions of each rotor.
     * @param rotors A vector containing the configuration for each rotor.
     * @param reflector Configuration for the reflector.
     * @param logger Optional logger for event reporting.
     * @throws std::invalid_argument If the number of rotors does not match the number of positions.
     * @throws std::runtime_error If initialization of transformers fails.
     */
    RotorBox(const std::vector<AlphabetIndex>& rotorPositions, const std::vector<RotorConfig>& rotors,
             const ReflectorConfig& reflector, ILogger* logger = nullptr);

    /**
     * @brief Constructor for the RotorBox class (move version).
     * @param rotorPositions A vector containing the initial positions of each rotor.
     * @param rotors A vector containing the configuration for each rotor.
     * @param reflector Configuration for the reflector.
     * @param logger Optional logger for event reporting.
     */
    RotorBox(std::vector<AlphabetIndex>&& rotorPositions, std::vector<RotorConfig>&& rotors,
             ReflectorConfig&& reflector, ILogger* logger = nullptr);

    /**
     * @brief Factory method to create a RotorBox.
     * Returns a Result to allow error handling without exceptions.
     *
     * @param rotorPositions A vector containing the initial positions of each rotor.
     * @param rotors A vector containing the configuration for each rotor.
     * @param reflector Configuration for the reflector.
     * @param logger Optional logger for event reporting.
     * @return enigma::Result<RotorBox> The created RotorBox or an error code.
     */
    static enigma::Result<RotorBox> create(const std::vector<AlphabetIndex>& rotorPositions,
                                           const std::vector<RotorConfig>& rotors, const ReflectorConfig& reflector,
                                           ILogger* logger = nullptr);
    static enigma::Result<RotorBox> create(std::vector<AlphabetIndex>&& rotorPositions,
                                           std::vector<RotorConfig>&& rotors, ReflectorConfig&& reflector,
                                           ILogger* logger = nullptr);

    // Disable Copy
    RotorBox(const RotorBox&) = delete;
    RotorBox& operator=(const RotorBox&) = delete;

    // Enable Move
    RotorBox(RotorBox&&) = default;
    RotorBox& operator=(RotorBox&&) = default;

    ~RotorBox() = default;

    /**
     * @brief Registers an observer to receive notifications.
     * @param observer The observer to register.
     */
    void registerObserver(IEnigmaObserver* observer);

    /**
     * @brief Removes an observer.
     * @param observer The observer to remove.
     */
    void removeObserver(IEnigmaObserver* observer);

    /**
     * @brief Prints the types of transformers in the transformer vector.
     * This function iterates through the transformer vector and prints the type of each transformer.
     */
    void printTransformers() const;

    /**
     * @brief Transforms the input key through the rotor box.
     *
     * @param input The input key to be transformed.
     * @return AlphabetIndex The transformed output key.
     */
    AlphabetIndex keyTransform(AlphabetIndex input);

    /**
     * @brief Sets the logger for the rotor box.
     * @param logger The logger to use.
     */
    void setLogger(ILogger* logger);
};

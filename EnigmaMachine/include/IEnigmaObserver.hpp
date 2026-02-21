#pragma once

#include "EnigmaCore_EXPORT.hpp"

/**
 * @brief Interface for observing Enigma Machine events.
 * Implement this interface to receive notifications about state changes.
 */
struct ENIGMACORE_EXPORT IEnigmaObserver {
    virtual ~IEnigmaObserver();

    /**
     * @brief Called when a rotor steps.
     * @param rotorIndex The index of the rotor (0 is the rightmost/fastest).
     * @param position The new position of the rotor (0-25).
     */
    virtual void onRotorStepped(int rotorIndex, int position) = 0;

    /**
     * @brief Called when a character is encrypted/decrypted.
     * @param input The original input character.
     * @param output The final output character.
     */
    virtual void onCharEncrypted(char input, char output) = 0;
};

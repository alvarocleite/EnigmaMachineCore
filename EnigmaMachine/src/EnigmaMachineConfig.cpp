/**
 * @file EnigmaMachineConfig.cpp
 * @brief Implementation of the EnigmaMachineConfig struct.
 */

#include "EnigmaMachineConfig.hpp"

#include <numeric>

RotorConfig::RotorConfig() { std::iota(wiring.begin(), wiring.end(), 0); }

ReflectorConfig::ReflectorConfig() {
    for (int i = 0; i < enigma::TRANSFORMER_SIZE; ++i) {
        wiring[i] = enigma::TRANSFORMER_SIZE - 1 - i;
    }
}

EnigmaMachineConfig::EnigmaMachineConfig() {
    for (auto& pair : plugBoardPairs) {
        pair.sourcePortIndex = -1;
        pair.destinationPortIndex = -1;
    }
}

enigma::EnigmaMachineData EnigmaMachineConfig::toData() const {
    enigma::EnigmaMachineData data = {};

    data.rotorCount = rotorCount;

    for (int i = 0; i < rotorCount && i < enigma::MAX_ROTORS; ++i) {
        data.rotorPositions[i] = rotorPositions[i];
        data.rotors[i].notchPosition = rotors[i].notchPosition;
        for (int j = 0; j < enigma::TRANSFORMER_SIZE; ++j) {
            data.rotors[i].wiring[j] = rotors[i].wiring[j];
        }
    }

    for (int i = 0; i < enigma::TRANSFORMER_SIZE; ++i) {
        data.reflector.wiring[i] = reflector.wiring[i];
    }

    for (int i = 0; i < enigma::MAX_PLUGBOARD_PAIRS; ++i) {
        data.plugBoard.pairs[i].sourcePortIndex = plugBoardPairs[i].sourcePortIndex;
        data.plugBoard.pairs[i].destinationPortIndex = plugBoardPairs[i].destinationPortIndex;
    }

    return data;
}

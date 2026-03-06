/**
 * @file EnigmaMachineConfig.cpp
 * @brief Implementation of the EnigmaMachineConfig struct.
 */

#include "EnigmaMachineConfig.hpp"

#include <numeric>

RotorConfig::RotorConfig() {
    wiring.resize(TRANSFORMER_SIZE);
    std::iota(wiring.begin(), wiring.end(), 0);
}

ReflectorConfig::ReflectorConfig() {
    wiring.resize(TRANSFORMER_SIZE);
    for (int i = 0; i < TRANSFORMER_SIZE; ++i) {
        wiring[i] = TRANSFORMER_SIZE - 1 - i;
    }
}

EnigmaMachineConfig::EnigmaMachineConfig() {
    for (auto& pair : plugBoardPairs) {
        pair.sourcePortIndex = -1;
        pair.destinationPortIndex = -1;
    }
}

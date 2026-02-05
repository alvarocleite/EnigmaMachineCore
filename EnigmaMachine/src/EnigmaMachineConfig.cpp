/**
 * @file EnigmaMachineConfig.cpp
 * @brief Implementation of the EnigmaMachineConfig struct.
 */

#include "EnigmaMachineConfig.hpp"

EnigmaMachineConfig::EnigmaMachineConfig() {
    for (auto& pair : plugBoardPairs) {
        pair.sourcePortIndex = -1;
        pair.destinationPortIndex = -1;
    }
}

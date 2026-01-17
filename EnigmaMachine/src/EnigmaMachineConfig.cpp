/**
 * @file EnigmaMachineConfig.cpp
 * @brief Implementation of the EnigmaMachineConfig struct.
 */

#include "EnigmaMachineConfig.hpp"

EnigmaMachineConfig::EnigmaMachineConfig() {
    for (auto& pair : plugBoardPairs) {
        pair.a = -1;
        pair.b = -1;
    }
}

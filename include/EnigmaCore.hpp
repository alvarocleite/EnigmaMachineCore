/**
 * @file EnigmaCore.hpp
 * @brief Public API gateway for the EnigmaCore library.
 *
 * This header serves as the single entry point for library consumers,
 * including all necessary headers to interact with the Enigma Machine.
 */

#pragma once

// Core Engine
#include "EnigmaMachine.hpp"
#include "EnigmaMachineConfig.hpp"
#include "IEnigmaObserver.hpp"

// Asset Management
#include "IAssetProvider.hpp"
#include "FileAssetProvider.hpp"

// Components (Optional for direct interaction)
#include "Rotor.hpp"
#include "Reflector.hpp"
#include "PlugBoard.hpp"
#include "RotorBox.hpp"

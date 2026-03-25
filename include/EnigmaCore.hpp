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
#include "EnigmaTypes.hpp"
#include "IEnigmaObserver.hpp"

// Asset Management
#include "IAssetProvider.hpp"

// Error Handling
#include "EnigmaError.hpp"

// POD Configuration
#include "EnigmaConfig.hpp"
#include "EnigmaData.hpp"

#!/bin/bash
set -e

# Path to the shared library
LIB_PATH=$1

if [ ! -f "$LIB_PATH" ]; then
    echo "Error: Library not found at $LIB_PATH"
    exit 1
fi

echo "Verifying Public API symbols in $LIB_PATH..."

# List of symbols that MUST be present (regex)
REQUIRED_SYMBOLS=(
    "EnigmaMachine::keyTransform"
    "EnigmaMachine::registerObserver"
    "EnigmaMachine::EnigmaMachine"
    "IEnigmaObserver::~IEnigmaObserver"
    "IAssetProvider::~IAssetProvider"
)

# List of symbols that MUST NOT be present (regex)
FORBIDDEN_SYMBOLS=(
    "Rotor::"
    "Reflector::"
    "PlugBoard::"
    "RotorBox::"
    "Transformer::"
    "EnigmaConfigLoader::"
)

# Get exported symbols
EXPORTED_SYMBOLS=$(nm -D "$LIB_PATH" | grep -E " T | W " | c++filt)

# Check Required
for sym in "${REQUIRED_SYMBOLS[@]}"; do
    if echo "$EXPORTED_SYMBOLS" | grep -q "$sym"; then
        echo "[PASS] Found required symbol: $sym"
    else
        echo "[FAIL] Missing required symbol: $sym"
        exit 1
    fi
done

# Check Forbidden
for sym in "${FORBIDDEN_SYMBOLS[@]}"; do
    if echo "$EXPORTED_SYMBOLS" | grep -q "$sym"; then
        echo "[FAIL] Found forbidden internal symbol: $sym"
        exit 1
    else
        echo "[PASS] Internal symbol hidden: $sym"
    fi
done

echo "API Validation Successful!"

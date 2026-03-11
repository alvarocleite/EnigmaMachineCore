#!/bin/bash
set -e

# Helper script to run EnigmaMachineCore through common scenarios
# for sanitizer verification (ASan, UBSan, MSan).

BUILD_DIR=${1:-build}
APP_BIN="$BUILD_DIR/EnigmaMachineCore"
ASSETS_DIR="$BUILD_DIR/assets"
CONFIG_FILE="$ASSETS_DIR/EnigmaMachineConfig1.toml"

if [ ! -f "$APP_BIN" ]; then
    echo "Error: EnigmaMachineCore binary not found in $BUILD_DIR"
    echo "Usage: ./scripts/run_sanitizers.sh [build_directory]"
    exit 1
fi

echo "--- Starting Sanitizer Check ---"

# Scenario 1: Help
echo "[Scenario 1] Running --help..."
$APP_BIN --help > /dev/null

# Scenario 2: Simple encoding/decoding
echo "[Scenario 2] Encoding and decoding message..."
$APP_BIN -c "$CONFIG_FILE" -a "$ASSETS_DIR" -m "THEQUICKBROWNFOXJUMPSOVERTHELAZYDOG" --encode --decode > /dev/null

# Scenario 3: Debug mode enabled
echo "[Scenario 3] Running in debug mode..."
$APP_BIN -c "$CONFIG_FILE" -a "$ASSETS_DIR" -m "ENIGMA" --debug > /dev/null

echo "--- Sanitizer Check Completed Successfully ---"
echo "If no output was produced above (other than scenario messages), no issues were detected."

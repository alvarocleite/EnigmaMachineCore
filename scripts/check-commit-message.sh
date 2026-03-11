#!/bin/bash

# Commit message file path is passed as the first argument
COMMIT_MSG_FILE=$1

# Check if commit message file exists
if [ ! -f "$COMMIT_MSG_FILE" ]; then
    echo "Error: Commit message file not found."
    exit 1
fi

# Get the content of the commit message
# We use awk to check if there are at least two paragraphs (title and body)
# separated by a blank line (RS="").
if ! awk 'BEGIN {RS=""; FS="\n"} END {if (NR >= 2) exit 0; else exit 1}' "$COMMIT_MSG_FILE"; then
    echo "Error: Commit message must have a title and a body separated by a blank line."
    echo "Current commit message format is incorrect."
    echo "Expected format:"
    echo "Title (50 chars or less)"
    echo ""
    echo "Detailed body explaining the changes..."
    exit 1
fi

exit 0

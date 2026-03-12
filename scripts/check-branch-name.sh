#!/bin/bash

# Allowed branch name patterns
ALLOWED_TYPES=("feature" "fix" "docs" "release")

# Get current branch name
CURRENT_BRANCH=$(git rev-parse --abbrev-ref HEAD)

# Skip for special branches like 'main' and 'development'
if [[ "$CURRENT_BRANCH" == "main" || "$CURRENT_BRANCH" == "development" ]]; then
    exit 0
fi

# Check if branch name starts with an allowed type followed by a /
for type in "${ALLOWED_TYPES[@]}"; do
    if [[ "$CURRENT_BRANCH" == "$type/"* ]]; then
        exit 0
    fi
done

echo "Error: Branch name '$CURRENT_BRANCH' is invalid."
echo "Branch names must start with one of: ${ALLOWED_TYPES[*]} followed by a slash (e.g., feature/my-feature)."
exit 1

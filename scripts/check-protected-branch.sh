#!/bin/bash

# Protected branches
PROTECTED_BRANCHES=("main" "development")

# Get current branch name
CURRENT_BRANCH=$(git rev-parse --abbrev-ref HEAD)

# Skip check in CI environments (like GitHub Actions)
# This allows CI to run pre-commit checks on protected branches themselves
if [ "$GITHUB_ACTIONS" == "true" ]; then
    exit 0
fi

# Check if current branch is in the protected list
for branch in "${PROTECTED_BRANCHES[@]}"; do
    if [ "$CURRENT_BRANCH" == "$branch" ]; then
        echo "Error: You are attempting to commit directly to a protected branch ($CURRENT_BRANCH)."
        echo "Please create a feature branch and submit a Pull Request instead."
        exit 1
    fi
done

exit 0

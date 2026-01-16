# Contributing to EnigmaMachineCore

Thank you for your interest in contributing to EnigmaMachineCore! We follow a strict git workflow to ensure code stability and quality. Please review the guidelines below before making changes.

## Branching Strategy

The repository uses the following branch structure:

*   **`main`**: The stable branch containing production-ready code. Direct commits to `main` are **forbidden**.
*   **`development`**: The integration branch for the next release. All feature branches are merged here first. Direct commits to `development` are **forbidden**.
*   **`feature/*`** or **`fix/*`**: Short-lived branches for specific features or bug fixes. This is where all work happens.

---

## Workflow for Contributors

### 1. Internal / Core Developers

If you have write access to the repository, follow this workflow:

1.  **Start from `development`**: Always create your feature branch from the latest `development` state.
    ```bash
    git checkout development
    git pull origin development
    git checkout -b feature/your-feature-name
    ```

2.  **Make your changes**: Implement your feature or fix.
    *   Ensure all tests pass locally.
    *   Follow the project's coding standards.

3.  **Submit a Pull Request (PR) to `development`**:
    *   Push your branch to the repository.
    *   Open a PR targeting the **`development`** branch.
    *   Fill out the PR template completely.

4.  **Review & Merge**:
    *   Wait for code review and CI checks to pass.
    *   Once approved, squash and merge into `development`.

5.  **Releasing to `main`**:
    *   Periodically, when `development` reaches a stable milestone, a maintainer will open a PR from **`development`** to **`main`**.

### 2. External Developers (Forks)

If you are contributing from outside the organization, please use the Fork & Pull model:

1.  **Fork the Repository**: Click the "Fork" button on the GitHub page to create your own copy.

2.  **Clone your Fork**:
    ```bash
    git clone https://github.com/YOUR_USERNAME/EnigmaMachineCore.git
    cd EnigmaMachineCore
    ```

3.  **Sync with Upstream**: Add the original repository as a remote to keep your fork updated.
    ```bash
    git remote add upstream https://github.com/alvarocleite/EnigmaMachineCore.git
    ```

4.  **Create a Branch**: Create a feature branch from the upstream `development` branch.
    ```bash
    git fetch upstream
    git checkout -b feature/your-feature-name upstream/development
    ```

5.  **Push & PR**:
    *   Push changes to your fork: `git push origin feature/your-feature-name`.
    *   Go to the original `EnigmaMachineCore` repository on GitHub.
    *   Open a new Pull Request.
    *   **Important**: Set the **base repository** branch to **`development`**.

## Releasing and Tagging

We use Git tags to mark significant milestones and stable releases of the project.

*   **Stable Releases Only**: Tags are only generated for commits on the **`main`** branch.
*   **Stability Requirement**: A tag must only be created when the codebase is confirmed stable, passing all integration tests and qualification checks.
*   **Milestones**: Each tag represents a specific version/milestone (e.g., `v1.0.0`) that users can rely on for production or industrial use.

## Pull Request Guidelines

*   **Template**: Please fill out the provided PR template.
*   **Scope**: Keep PRs focused on a single issue or feature.
*   **Tests**: Verify that new code is covered by unit tests.
*   **CI**: Ensure all GitHub Actions workflows (Build, Format, Static Analysis) pass.

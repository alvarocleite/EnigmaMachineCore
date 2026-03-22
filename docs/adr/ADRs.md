# Architectural Decision Records (ADRs)

This directory contains records of significant architectural decisions made for the EnigmaMachineCore project.

## Index

* [ADR 001: Dependency Injection for Assets (IAssetProvider)](001-dependency-injection-for-assets.md)
* [ADR 002: Signal Path Strategy (Alphabet Index vs. Raw Chars)](002-signal-path-strategy.md)
* [ADR 003: Error Handling Strategy (Transition to std::expected)](003-error-handling-strategy.md)
* [ADR 004: Split Signal Path Optimization (Branchless Signal Path)](004-split-signal-path-optimization.md)
* [ADR 005: Modulo Optimization (Conditional Subtraction)](005-modulo-optimization.md)
* [ADR 006: Plain Old Data (POD) Configuration DTOs](006-POD-DTOs.md) - Enable embedded/WASM without dynamic allocation
* [ADR 007: Result Type Implementation](007-result-type.md) - std::expected via expected-lite + EnigmaError enum

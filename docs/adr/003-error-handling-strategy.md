# ADR 003: Error Handling Strategy (Transition to std::expected)

## Status
Accepted — **Implemented by [ADR 007](007-result-type.md)**

## Context
The Enigma Machine Core is designed to be universal, and it targets platforms that do not support exceptions (`-fno-exceptions`) or where exceptions are discouraged for performance reasons (e.g., Embedded systems, WASM).

Currently, the engine uses:
1.  **Exceptions:** `std::runtime_error` and `std::invalid_argument` are thrown in constructors and configuration loaders.
2.  **Boolean Returns/Logging:** Some internal logic reports errors via `std::cerr` or simple boolean returns.

## Decision
We will transition from C++ exceptions to the **`std::expected`** (C++23) pattern for reporting errors in public and internal APIs.

1.  **Value-Error Return:** Public-facing methods like `keyTransform` and configuration loaders will return a `Result<T>` (aliased to `std::expected<T, ErrorCode>`).
2.  **No-Exceptions Compatibility:** This approach allows us to compile the engine with `-fno-exceptions` without losing error-reporting capability.
3.  **Exception-Free Core:** After the transition, all `throw` statements in the core logic will be removed or conditionally enabled for rich targets only.
4.  **Error Codes:** We will define a comprehensive set of `ErrorCode` values representing configuration failures, validation errors, and runtime issues.

## Consequences
*   **Portability:** The engine becomes fully compatible with constrained targets (embedded, WASM).
*   **API Changes:** Most public methods will have their return signatures updated.
*   **Verbosity:** Error checking becomes explicit in the caller, similar to Rust's `Result` or Go's error handling.
*   **Dependency:** Since we use C++20, we may need a backport (like `tl::expected`) if C++23 is not yet available in all target compilers.

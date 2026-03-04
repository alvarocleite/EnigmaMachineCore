# ADR 004: Split Signal Path Optimization (Branchless Signal Path)

## Status
Accepted

## Context
The historical Enigma Machine's signal path flows forward through rotors, hits a reflector, and returns in reverse. Initially, the system used a single `transform(position, reverse)` virtual method. Every character transformation required checking the `reverse` flag, introducing a "branch" in the CPU's execution pipeline millions of times per message.

## Decision
We will use **Dedicated Forward and Reverse Signal Paths** by splitting the transformation into `transformForward(position)` and `transformReverse(position)` methods.

1.  **CPU Performance:** Eliminates the conditional branch (`if (reverse)`) inside the tightest loop of the engine, improving CPU instruction pipelining and branch prediction.
2.  **Explicit Intent:** Code reading the signal flow (in `RotorBox::keyTransform`) becomes more readable and explicit about which direction the signal is traveling.
3.  **Reflector Symmetry:** Reflectors, which only have a "forward" entry in a physical machine, can explicitly return an error or a placeholder in the `transformReverse` path, making the implementation more faithful to the physical hardware.
4.  **Static Polymorphism Readiness:** This split is a prerequisite for future C++20 template-based optimizations, where the compiler can inline the specific forward or reverse path logic at compile-time.

## Consequences
*   **Virtual Table Size:** Increases the size of the vtable for `Transformer` by adding two more virtual functions.
*   **Code Duplication:** Some logic is duplicated between `transformForward` and `transformReverse` (coordinate shifts), but the performance gain in the hot path outweighs the maintenance cost.
*   **Method Maintenance:** Any future change to the coordinate system or signal mapping must be applied consistently to both forward and reverse methods.

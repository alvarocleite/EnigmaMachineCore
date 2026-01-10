#include "PlugBoard.hpp"
#include "config.hpp"
#include <iostream>

PlugBoard::PlugBoard() {
    for (int i = 0; i < TRANSFORMER_SIZE; ++i) {
        mapping[i] = i;
    }
}

/**
 * @details Validates the provided pairs before mapping them.
 * A port is 'unused' if it maps to itself.
 * If either port 'a' or 'b' is already mapped to something else, a conflict is reported 
 * because a socket cannot have two plugs.
 */
PlugBoard::PlugBoard(std::array<Pair_t, PLUGBOARD_MAX_PAIRS> pairs) : PlugBoard() {
    for (const auto& pair : pairs) {
        int a = pair.a;
        int b = pair.b;

        if (a < 0 || a >= TRANSFORMER_SIZE || b < 0 || b >= TRANSFORMER_SIZE) {
            continue;
        }

        if (a == b) {
            continue;
        }

        if (mapping[a] != a || mapping[b] != b) {
            std::cerr << "Warning: PlugBoard conflict for pair (" << a << ", " << b << "). Skipping." << std::endl;
            continue;
        }

        mapping[a] = b;
        mapping[b] = a;
    }
}

PlugBoard::~PlugBoard() {}

/**
 * @details Performs a character swap using the pre-calculated mapping table.
 * @internal This operation is O(1) and is performed twice for every key press in the EnigmaMachine.
 */
int PlugBoard::swap(int key) const {
    if (key < 0 || key >= TRANSFORMER_SIZE) {
        return key; 
    }
    return mapping[key];
}
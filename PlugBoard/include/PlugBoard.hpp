#pragma once

#include <array>

#include "../../config/config.hpp"

typedef struct {
    int a;
    int b;
} Pair_t;

class PlugBoard
{
private:
    std::array<Pair_t, PLUGBOARD_MAX_PAIRS> pairs; // Assuming a maximum of 10 pairs for the plugboard
    int pairCount = 0; // Current number of pairs in the plugboard
    bool isPairValid(int a, int b);
    bool addPair(int a, int b);

public:
    PlugBoard();
    PlugBoard(std::array<Pair_t, PLUGBOARD_MAX_PAIRS> pairs);
    ~PlugBoard();

    int swap(int key);
};

#pragma once

#include <array>

#include "config.hpp"

struct Pair_t {
    int a;
    int b;
};

class PlugBoard
{
private:
    std::array<int, TRANSFORMER_SIZE> mapping; // Direct mapping: mapping[Input] = Output

public:
    PlugBoard();
    PlugBoard(std::array<Pair_t, PLUGBOARD_MAX_PAIRS> pairs);
    ~PlugBoard();

    int swap(int key) const;
};
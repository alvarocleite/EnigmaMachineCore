#pragma once

#include <vector>
#include <memory>

#include "../Transformer/include/Transformer.hpp"

class RotorBox{
private:
    int nRotorCount;
    std::vector<int> rotorPositions;
    std::vector<std::unique_ptr<Transformer>> transformerVec;
    int initTransformerVec(int nRotorCount);
    int updateRotors();
public:
    RotorBox();
    RotorBox(int nRotorCount, const std::vector<int> &rotorPositions);
    ~RotorBox();
    void printTransformerVec();

    int keyTransform(int input);
};

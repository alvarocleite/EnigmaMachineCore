#pragma once

#include "../../include/Transformer.hpp"

class Normalizer : public Transformer{
private:
    int lRotationCount;
    int rRotationCount;
    bool initTransformLUT(){return true;};
public:
    Normalizer();
    ~Normalizer();
    int transform(int position, int &newPosition, bool reverse = false);
    int setRotationCount(const int lRotation, const int rRotation);
};

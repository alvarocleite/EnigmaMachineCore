#pragma once

#include "../../include/Transformer.hpp"

class Reflector : public Transformer {
private:
    void initTransformLUT();
public:
    Reflector();
    ~Reflector();
    int transform(int position, int &newPosition, bool reverse = false);
};

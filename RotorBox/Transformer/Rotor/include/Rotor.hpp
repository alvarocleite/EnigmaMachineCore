#pragma once

#include "../../include/Transformer.hpp"

class Rotor: public Transformer {
private:
    int notchPosition;
    bool initTransformLUT();
public:
    Rotor();
    ~Rotor();
    int transform(int position, int &newPosition, bool reverse = false);
};

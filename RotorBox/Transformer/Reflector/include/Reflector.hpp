#pragma once

#include "../../include/Transformer.hpp"

class Reflector : public Transformer {
private:
    bool initTransformLUT();
public:
    Reflector();
    ~Reflector();
    int transform(int position, bool reverse = false);
    int rotate() { return 0; } // Reflectors do not rotate
};

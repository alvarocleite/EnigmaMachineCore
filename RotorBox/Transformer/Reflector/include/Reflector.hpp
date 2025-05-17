#pragma once

#include "../../include/Transformer.hpp"

class Reflector : public Transformer {
private:
    bool initTransformLUT(std::string fileName);
public:
    Reflector(std::string fileName);
    ~Reflector();
    int transform(int position, bool reverse = false);
    int rotate() { return 0; } // Reflectors do not rotate
};

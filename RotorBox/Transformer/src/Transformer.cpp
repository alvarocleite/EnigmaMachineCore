#include "../include/Transformer.hpp"

#include <fstream>
#include <iostream>
#include <string>

Transformer::Transformer(){
    type = notDefined;
}

Transformer::~Transformer(){}

int Transformer::sizeOfTransformLUT(){
    return sizeof(transformLUT) / sizeof(transformLUT[0][0]);
}

transformerType Transformer::getType(){
    return type;
}

int Transformer::initForwardTransformLUT(std::string fileName){
    // Open the file
    std::ifstream file;
    file.open(fileName);

    if (!file.is_open()) {
        std::cerr << "Error opening file" << std::endl;
        return false;
    }

    // Initialize forward transformation vector from file
    int i = 0;
    int a;
    while(file >> a){
        transformLUT[0][i] = a;
        i++;
        if (i == TRANSFORMER_SIZE){
            break;
        }
    }

    // close the file
    file.close();
    return 0;
}

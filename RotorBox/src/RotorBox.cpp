#include <iostream>
#include <string>

#include "../include/RotorBox.hpp"
#include "../Transformer/Reflector/include/Reflector.hpp"
#include "../Transformer/Rotor/include/Rotor.hpp"

RotorBox::RotorBox(){
    nRotorCount = 3;
    for(int i = 0; i < nRotorCount; i++){
        rotorPositions.push_back(0);
    }
    initTransformerVec(nRotorCount, std::vector<std::string> {"../assets/Rotor1.txt", 
                                         "../assets/Rotor2.txt", 
                                         "../assets/Rotor3.txt", 
                                         "../assets/Reflector.txt"});
}

RotorBox::RotorBox(int nRotorCount, const std::vector<int> &rotorPositions, const std::vector<std::string> &rotorFiles){
    if (nRotorCount != rotorPositions.size()){
        std::cerr << "Error: Number of rotors and number of rotor positions do not match." << std::endl;
        return;
    }
    
    this->nRotorCount = nRotorCount;
    for(auto &position : rotorPositions){
        this->rotorPositions.push_back(position);
    }
    
    initTransformerVec(nRotorCount, rotorFiles);
}

RotorBox::~RotorBox(){}

int RotorBox::initTransformerVec(int nRotorCount, const std::vector<std::string> &rotorFiles){
    int index = 0;
    while(index < nRotorCount){
        transformerVec.push_back(std::make_unique<Rotor>(rotorFiles[index]));
        index++;
    };
    //transformerVec.push_back(std::make_unique<Reflector>(std::string("../assets/Reflector.txt")));
    transformerVec.push_back(std::make_unique<Reflector>(rotorFiles[nRotorCount]));

    return 0;
}

void RotorBox::printTransformerVec(){
    for(auto &transformer : transformerVec){
        std::cout << "Transformer Type: " << transformer->getType() << std::endl;
    }
}

int RotorBox::keyTransform(int input){
    // update rotors position rotating
    updateRotors();

    // transform trough rotors forward
    bool reverse = false;
    int newPosition = input;
    for(int i = 0; i < nRotorCount; i++){
        newPosition = transformerVec[i]->transform(newPosition, reverse);
    }

    // reflector
    newPosition = transformerVec[nRotorCount]->transform(newPosition, reverse);

    // transform trough rotors in reverse
    reverse = true;
    for(int i = nRotorCount - 1; i >= 0; i--){ 
        newPosition = transformerVec[i]->transform(newPosition, reverse);
    }

    return newPosition;
}

int RotorBox::updateRotors(){
    int rotorIx = 0;
    int isNotch = 0;
    do{
        isNotch = transformerVec[rotorIx]->rotate();
        rotorIx++;
    } while (rotorIx < nRotorCount && isNotch == 1);

    return 0;
}

#include <iostream>
#include <string>

#include "../include/RotorBox.hpp"
#include "../Transformer/Reflector/include/Reflector.hpp"
#include "../Transformer/Rotor/include/Rotor.hpp"

/**
 * @brief Constructor for the RotorBox class.
 * Initializes the rotor box with a default number of rotors (3) and their positions (all set to 0).
 * Also initializes the transformer vector with default rotor and reflector files.
 */
RotorBox::RotorBox(){
    nRotorCount = 3;
    for(int i = 0; i < nRotorCount; i++){
        rotorPositions.push_back(0);
    }
    initTransformerVec(nRotorCount, std::vector<std::string> {"../assets/Rotor1.toml", 
                                         "../assets/Rotor2.toml", 
                                         "../assets/Rotor3.toml", 
                                         "../assets/Reflector.toml"});
}

/**
 * @brief Constructor for the RotorBox class.
 * Initializes the rotor box with a specified number of rotors, their positions, and corresponding files.
 * 
 * @param nRotorCount The number of rotors in the rotor box.
 * @param rotorPositions A vector containing the initial positions of each rotor.
 * @param rotorFiles A vector containing the file names for each rotor and reflector.
 */
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

/**
 * @brief Initializes the transformer vector with rotors and a reflector.
 * 
 * @param nRotorCount The number of rotors to be initialized.
 * @param rotorFiles A vector containing the file names for each rotor and reflector.
 * @return int Returns 0 on success.
 */
int RotorBox::initTransformerVec(int nRotorCount, const std::vector<std::string> &rotorFiles){
    int index = 0;
    while(index < nRotorCount){
        transformerVec.push_back(std::make_unique<Rotor>(rotorFiles[index]));
        index++;
    };
    transformerVec.push_back(std::make_unique<Reflector>(rotorFiles[nRotorCount]));

    return 0;
}

/**
 * @brief Prints the types of transformers in the transformer vector.
 * This function iterates through the transformer vector and prints the type of each transformer.
 */
void RotorBox::printTransformerVec(){
    for(auto &transformer : transformerVec){
        std::cout << "Transformer Type: " << transformer->getType() << std::endl;
    }
}

/**
 * @brief Transforms the input key through the rotor box.
 * This function updates the rotor positions, transforms the input through the rotors and reflector,
 * and returns the transformed output.
 * 
 * @param input The input key to be transformed.
 * @return int The transformed output key.
 */
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

/**
 * @brief Updates the positions of the rotors.
 * This function rotates each rotor in the transformer vector and checks for notch positions.
 * If a rotor reaches its notch position, it triggers the rotation of the next rotor.
 * 
 * @return int Returns 0 on success.
 */
int RotorBox::updateRotors(){
    int rotorIx = 0;
    int isNotch = 0;
    do{
        isNotch = transformerVec[rotorIx]->rotate();
        rotorIx++;
    } while (rotorIx < nRotorCount && isNotch == 1);

    return 0;
}

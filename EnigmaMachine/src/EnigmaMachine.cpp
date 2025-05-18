#include <iostream>
#include <vector>

#include "../include/EnigmaMachine.hpp"
#include "../../config/config.hpp"

EnigmaMachine::EnigmaMachine()
    : rotorBox(3, std::vector<int> {0, 0, 0},
              std::vector<std::string> {assetsDir + "Rotor1.txt", 
                                        assetsDir + "Rotor2.txt", 
                                        assetsDir + "Rotor3.txt", 
                                        assetsDir + "Reflector.txt"})
{}

EnigmaMachine::~EnigmaMachine(){}

int EnigmaMachine::keyTransform(int input){
    return rotorBox.keyTransform(input);
}

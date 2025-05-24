#include <iostream>
#include <vector>

#include "../include/EnigmaMachine.hpp"
#include "../../config/config.hpp"

EnigmaMachine::EnigmaMachine()
    : rotorBox(3, std::vector<int> {0, 0, 0},
              std::vector<std::string> {assetsDir + "Rotor1.toml", 
                                        assetsDir + "Rotor2.toml", 
                                        assetsDir + "Rotor3.toml", 
                                        assetsDir + "Reflector.toml"})
{}

EnigmaMachine::EnigmaMachine(int nRotorCount, const std::vector<int> &rotorPositions, const std::vector<std::string> &rotorFiles)
    : rotorBox(nRotorCount, rotorPositions, rotorFiles)
{}

EnigmaMachine::~EnigmaMachine(){}

int EnigmaMachine::keyTransform(int input){
    return rotorBox.keyTransform(input);
}

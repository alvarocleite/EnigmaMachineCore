#include <iostream>
#include <vector>

#include "../include/EnigmaMachine.hpp"

EnigmaMachine::EnigmaMachine()
    : rotorBox(3, std::vector<int> {0, 0, 0})
{}

EnigmaMachine::~EnigmaMachine(){}

int EnigmaMachine::keyTransform(int input){
    return rotorBox.keyTransform(input);
}

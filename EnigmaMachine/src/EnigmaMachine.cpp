#include <iostream>
#include <vector>

#include "../include/EnigmaMachine.hpp"

EnigmaMachine::EnigmaMachine()
    : rotorBox(3, std::vector<int> {0, 0, 0},
              std::vector<std::string> {"../assets/Rotor1.txt", 
                                        "../assets/Rotor2.txt", 
                                        "../assets/Rotor3.txt", 
                                        "../assets/Reflector.txt"})
{}

EnigmaMachine::~EnigmaMachine(){}

int EnigmaMachine::keyTransform(int input){
    return rotorBox.keyTransform(input);
}

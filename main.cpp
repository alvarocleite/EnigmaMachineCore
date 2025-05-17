
#include <iostream>
#include "./EnigmaMachine/include/EnigmaMachine.hpp"

int main(){
    int position = 0;
    int newPosition;
    EnigmaMachine enigmaMachine;
    EnigmaMachine enigmaMachine2;
    std::cout << "Hello, World!" << std::endl;
    int input = 3;
    newPosition = enigmaMachine.keyTransform(input);
    std::cout << "Input: " << input << ", Output: " << newPosition << std::endl;
    int input2 = 4;
    newPosition = enigmaMachine2.keyTransform(input2);
    std::cout << "Input: " << input2 << ", Output: " << newPosition << std::endl;
    return 0;
}

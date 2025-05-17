
#include <iostream>
#include "./EnigmaMachine/include/EnigmaMachine.hpp"

int main(){
    int position = 0;
    int newPosition;
    EnigmaMachine enigmaMachineEncode;
    EnigmaMachine enigmaMachineDecode;
    
    std::cout << "Ecode: \"HELLOWORLD\"" << std::endl;
    char input[11] = "HELLOWORLD"; 
    for (int i = 0; i < 10; i++){
        newPosition = enigmaMachineEncode.keyTransform(input[i] - 'A');
        std::cout << "Input: " << input[i] << ", Output: " << (char)(newPosition + 'A') << std::endl;
    }
    
    std::cout << "Decode: \"EHPPMBMKPA\"" << std::endl;
    char input2[11] = "EHPPMBMKPA";
    for (int i = 0; i < 10; i++){
        newPosition = enigmaMachineDecode.keyTransform(input2[i] - 'A');
        std::cout << "Input: " << input2[i] << ", Output: " << (char)(newPosition + 'A') << std::endl;
    }
    return 0;
}

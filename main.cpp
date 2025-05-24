
#include <iostream>
#include "./EnigmaMachine/include/EnigmaMachine.hpp"

int main(){
    EnigmaMachine enigmaMachineEncode;
    EnigmaMachine enigmaMachineDecode;
   
    char msg2encode[11] = "HELLOWORLD";
    std::cout << "Encoding message: " << msg2encode << std::endl;

    char msg2decode[11];    
    for (int i = 0; i < 10; i++){
        msg2decode[i] = enigmaMachineEncode.keyTransform(msg2encode[i] - 'A') + 'A';
        std::cout << "Input: " << msg2encode[i] << ", Output: " << msg2decode[i] << std::endl;
    }
    msg2decode[10] = '\0'; // Null-terminate the string

    std::cout << "Decoding message: " << msg2decode << std::endl;

    char decodedMsg[11];
    for (int i = 0; i < 10; i++){
        decodedMsg[i] = enigmaMachineDecode.keyTransform(msg2decode[i] - 'A') + 'A';
        std::cout << "Input: " << msg2decode[i] << ", Output: " << decodedMsg[i] << std::endl;
    }

    bool isEqual = true;
    for (int i = 0; i < 10; i++){
        if (msg2encode[i] != decodedMsg[i]){
            isEqual = false;
            break;
        }
    }

    decodedMsg[10] = '\0'; // Null-terminate the string
    std::cout << "Original message: " << msg2encode << std::endl;
    std::cout << "Decoded message: " << decodedMsg << std::endl;

    if (isEqual){
        std::cout << "Decoded message matches the original message!" << std::endl;
    } else {
        std::cout << "Decoded message does not match the original message!" << std::endl;
    }

    std::cout << "End of program." << std::endl;
    return 0;
}

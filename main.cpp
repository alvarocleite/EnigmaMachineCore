
#include <iostream>
#include <string>
#include "./EnigmaMachine/include/EnigmaMachine.hpp"

#include "config/config.hpp"

/**
 * @brief Main function to demonstrate the encoding and decoding of a message using the Enigma machine.
 * It encodes a message, decodes it back, and checks if the decoded message matches the original.
 */
int main(){
    EnigmaMachine enigmaMachineEncode(assetsDir + "EnigmaMachineConfig1.toml");
    EnigmaMachine enigmaMachineDecode(assetsDir + "EnigmaMachineConfig1.toml");
   
    std::string msg2encode = "HELLOWORLD";
    std::cout << "Encoding message: " << msg2encode << std::endl;

    std::string msg2decode = "";    
    for (char c : msg2encode){
        char output = enigmaMachineEncode.keyTransform(c - 'A') + 'A';
        msg2decode += output;
        std::cout << "Input: " << c << ", Output: " << output << std::endl;
    }

    std::cout << "Decoding message: " << msg2decode << std::endl;

    std::string decodedMsg = "";
    for (char c : msg2decode){
        char output = enigmaMachineDecode.keyTransform(c - 'A') + 'A';
        decodedMsg += output;
        std::cout << "Input: " << c << ", Output: " << output << std::endl;
    }

    std::cout << "Original message: " << msg2encode << std::endl;
    std::cout << "Decoded message:  " << decodedMsg << std::endl;

    if (msg2encode == decodedMsg){
        std::cout << "Decoded message matches the original message!" << std::endl;
    } else {
        std::cout << "Decoded message does not match the original message!" << std::endl;
    }

    std::cout << "End of program." << std::endl;
    return 0;
}

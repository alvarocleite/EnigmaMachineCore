#include <gtest/gtest.h>
#include "EnigmaMachine.hpp"
#include "config.hpp"
#include <string>
#include <vector>

class EnigmaMachineTests : public ::testing::Test {
protected:
    const std::string configPath = "assets/EnigmaMachineConfig1.toml";

    // Helper to encrypt a string
    std::string encryptString(EnigmaMachine& machine, const std::string& input) {
        std::string output = "";
        for (char c : input) {
            int val = c - 'A';
            int res = machine.keyTransform(val);
            output += (char)(res + 'A');
        }
        return output;
    }
};

TEST_F(EnigmaMachineTests, Initialization) {
    EXPECT_NO_THROW({
        EnigmaMachine machine(configPath, assetsDir);
    });
}

TEST_F(EnigmaMachineTests, BasicEncryption) {
    EnigmaMachine machine(configPath, assetsDir);
    int res = machine.keyTransform(0); // 'A'
    EXPECT_GE(res, 0);
    EXPECT_LT(res, 26);
    // Deterministic check:
    // With current Config1 (Rotors 1,2,3 at 6,18,1, Plugs 4-7...), 'A' maps to something specific.
    // Let's just ensure it's consistent.
    EnigmaMachine m2(configPath, assetsDir);
    EXPECT_EQ(m2.keyTransform(0), res); 
}

TEST_F(EnigmaMachineTests, StringEncryption) {
    EnigmaMachine machine(configPath, assetsDir);
    std::string input = "AAAAA";
    std::string output = encryptString(machine, input);
    
    // 1. Output length matches input
    EXPECT_EQ(output.length(), input.length());
    
    // 2. Output is not just same char repeated (rotors are moving)
    bool allSame = true;
    for (size_t i = 1; i < output.length(); ++i) {
        if (output[i] != output[0]) allSame = false;
    }
    EXPECT_FALSE(allSame) << "Enigma output should vary for repeated input due to rotor stepping.";
}

TEST_F(EnigmaMachineTests, Reciprocity) {
    std::string plain = "HELLOWORLD";
    
    // 1. Encrypt
    EnigmaMachine mEnc(configPath, assetsDir);
    std::string cipher = encryptString(mEnc, plain);
    
    // 2. Decrypt (New machine with same initial state)
    EnigmaMachine mDec(configPath, assetsDir);
    std::string recovered = encryptString(mDec, cipher);
    
    EXPECT_EQ(recovered, plain) << "Decryption failed to recover plaintext.";
}

TEST_F(EnigmaMachineTests, PlugBoardEffect) {
    // 1. Machine WITH Plugboard (from file)
    EnigmaMachine mWithPlugs(configPath, assetsDir);
    
    // 2. Machine WITHOUT Plugboard (manual construction)
    // Need to match the file's rotor config: Rotors 1,2,3, Positions 6,18,1
    // Note: Config has positions [6, 18, 1]
    std::vector<int> positions = {6, 18, 1};
    std::vector<std::string> files = {
        assetsDir + "Rotor1.toml",
        assetsDir + "Rotor2.toml",
        assetsDir + "Rotor3.toml",
        assetsDir + "Reflector.toml"
    };
    
    EnigmaMachine mNoPlugs(3, positions, files);
    
    // The config has plugs: 4-7 (E-H), 18-20 (S-U), etc.
    // Let's encrypt 'E' (4). 
    // mWithPlugs will swap E->H BEFORE entering rotors.
    // mNoPlugs will send E directly to rotors.
    // The results should likely differ.
    
    int input = 4; // 'E'
    int out1 = mWithPlugs.keyTransform(input);
    int out2 = mNoPlugs.keyTransform(input);
    
    EXPECT_NE(out1, out2) << "Plugboard should alter the encryption path.";
}
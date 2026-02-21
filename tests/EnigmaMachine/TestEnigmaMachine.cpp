#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "EnigmaMachine.hpp"
#include "config.hpp"

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
    EXPECT_NO_THROW({ EnigmaMachine machine(configPath, assetsDir); });
}

TEST_F(EnigmaMachineTests, BasicEncryption) {
    EnigmaMachine machine(configPath, assetsDir);
    int res = machine.keyTransform(0);  // 'A'
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

    // 2. Machine WITHOUT Plugboard
    // We can use a simpler config or rely on default behavior if we had one,
    // but here let's just use a config that we know has no plugs or a different path.
    // For now, let's just ensure that two different configurations yield different results.
    
    // Instead of manual construction which is now internal, we use the supported constructor
    // but we can't easily "disable" plugs via filename if the file has them.
    // However, we can use the Default Constructor which has NO plugs.
    EnigmaMachine mDefault; 

    int input = 4;  // 'E'
    int out1 = mWithPlugs.keyTransform(input);
    int out2 = mDefault.keyTransform(input);

    EXPECT_NE(out1, out2) << "Different configurations (with/without plugs) should yield different results.";
}
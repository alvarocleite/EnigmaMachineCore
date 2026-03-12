#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "EnigmaMachine.hpp"
#include "config.hpp"

class HistoricalVectorTests : public ::testing::Test {
protected:
    const std::string historicalAssetPath = "assets/historical/";

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

/**
 * @brief Standard Enigma I check: AAAAA with Rotors I, II, III (Left-to-Right) and Reflector B.
 *
 * Note: In RotorBox, the rotors are stored from index 0 to rotorCount-1.
 * Index 0 is the rightmost (fast) rotor.
 * Historically, for Enigma I, Rotor III is the rightmost, Rotor II middle, Rotor I leftmost.
 *
 * Result: BDZGO (Verified with EnigmaCore implementation)
 */
TEST_F(HistoricalVectorTests, StandardAAAAA_BDZGO) {
    std::string configName = historicalAssetPath + "EnigmaI_Historical_Config.toml";
    EnigmaMachine machine(configName, historicalAssetPath);

    std::string input = "AAAAA";
    std::string expected = "BDZGO";
    std::string output = encryptString(machine, input);

    EXPECT_EQ(output, expected);
}

/**
 * @brief Symmetrical check: BDZGO should decrypt to AAAAA with same settings.
 */
TEST_F(HistoricalVectorTests, ReciprocalBDZGO) {
    std::string configName = historicalAssetPath + "EnigmaI_Historical_Config.toml";
    EnigmaMachine machine(configName, historicalAssetPath);

    std::string input = "BDZGO";
    std::string expected = "AAAAA";
    std::string output = encryptString(machine, input);

    EXPECT_EQ(output, expected);
}

/**
 * @brief Verify long string encryption matches established simulator baseline.
 * Input: THEQUICKBROWNFOXJUMPSOVERTHELAZYDOG
 * Expected: OPCILLAZFXLQTDNLGGLEKDIZOKQKGXIEZKD
 */
TEST_F(HistoricalVectorTests, LongStringVerification) {
    std::string configName = historicalAssetPath + "EnigmaI_Historical_Config.toml";
    EnigmaMachine machine(configName, historicalAssetPath);

    std::string input = "THEQUICKBROWNFOXJUMPSOVERTHELAZYDOG";
    std::string expected = "OPCILLAZFXLQTDNLGGLEKDIZOKQKGXIEZKD";
    std::string output = encryptString(machine, input);

    EXPECT_EQ(output, expected);
}

/**
 * @brief OCAML Test Vector: Rotors I, II, III (Left-to-Right), RefB, Pos: F, U, N, Plugs: A-Z.
 *
 * Historical (L-R): I(F), II(U), III(N)
 * RotorBox (R-L):  III(N), II(U), I(F)  -> [13, 20, 5]
 *
 * Input: YNGXQ
 * Expected: OCAML
 */
TEST_F(HistoricalVectorTests, OCAMLVector) {
    std::string configName = historicalAssetPath + "EnigmaI_OCAML_Config.toml";
    EnigmaMachine machine(configName, historicalAssetPath);

    std::string input = "YNGXQ";
    std::string expected = "OCAML";
    std::string output = encryptString(machine, input);

    EXPECT_EQ(output, expected);
}

/**
 * @brief Verify that encryption and decryption are reciprocal using historical settings.
 */
TEST_F(HistoricalVectorTests, HistoricalReciprocity) {
    std::string configName = historicalAssetPath + "EnigmaI_Historical_Config.toml";

    std::string plain = "SECRETENIGMAMESSAGE";

    // 1. Encrypt
    EnigmaMachine mEnc(configName, historicalAssetPath);
    std::string cipher = encryptString(mEnc, plain);

    // 2. Decrypt
    EnigmaMachine mDec(configName, historicalAssetPath);
    std::string recovered = encryptString(mDec, cipher);

    EXPECT_EQ(recovered, plain);
}

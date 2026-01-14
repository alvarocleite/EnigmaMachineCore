#include <gtest/gtest.h>
#include "EnigmaMachineConfig.hpp"
#include "config.hpp"

class EnigmaMachineConfigTests : public ::testing::Test {
protected:
    const std::string validConfigPath = "assets/EnigmaMachineConfig1.toml";
    const std::string invalidConfigPath = "assets/non_existent.toml";
};

TEST_F(EnigmaMachineConfigTests, LoadValidConfig) {
    EnigmaMachineConfig config;
    EXPECT_NO_THROW({ config = EnigmaMachineConfig::load(validConfigPath, assetsDir); });

    EXPECT_EQ(config.getRotorCount(), 3);

    std::vector<int> expectedPositions = {6, 18, 1};
    EXPECT_EQ(config.getRotorPositions(), expectedPositions);

    const auto& rotors = config.getRotors();
    EXPECT_EQ(rotors.size(), 3);

    const auto& pairs = config.getPlugBoardPairs();
    // Check a few pairs
    // { from =  4, to =  7 }
    // { from = 18, to = 20 }
    bool foundFirst = false;
    bool foundSecond = false;

    // Note: The array size is PLUGBOARD_MAX_PAIRS. Unused are (-1, -1).
    for (const auto& p : pairs) {
        if (p.a == 4 && p.b == 7) foundFirst = true;
        if (p.a == 18 && p.b == 20) foundSecond = true;
    }
    EXPECT_TRUE(foundFirst);
    EXPECT_TRUE(foundSecond);
}

TEST_F(EnigmaMachineConfigTests, LoadInvalidConfig) {
    EXPECT_THROW({ EnigmaMachineConfig::load(invalidConfigPath, assetsDir); }, std::exception);
}

TEST_F(EnigmaMachineConfigTests, RotorConfigProperties) {
    EnigmaMachineConfig config = EnigmaMachineConfig::load(validConfigPath, assetsDir);
    const auto& rotors = config.getRotors();
    ASSERT_FALSE(rotors.empty());

    // Check properties of the first loaded rotor (Rotor1)
    // We expect size 26
    EXPECT_EQ(rotors[0].wiring.size(), 26);
    EXPECT_GE(rotors[0].notchPosition, 0);
    EXPECT_LT(rotors[0].notchPosition, 26);
}

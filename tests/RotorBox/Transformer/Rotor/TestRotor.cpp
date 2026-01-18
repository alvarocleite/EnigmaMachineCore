#include <gtest/gtest.h>
#include "EnigmaConfigLoader.hpp"
#include "EnigmaMachineConfig.hpp"
#include "FileAssetProvider.hpp"
#include "Rotor.hpp"

class RotorTests : public ::testing::Test {
protected:
    const std::string configPath = "assets/Rotor1.toml";
    RotorConfig config;

    void SetUp() override {
        FileAssetProvider provider;
        config = EnigmaConfigLoader::loadRotor(provider, configPath);
    }
};

TEST_F(RotorTests, InitializationAndType) {
    Rotor rotor(config);
    EXPECT_EQ(rotor.getType(), TransformerType::Rotor);
}

TEST_F(RotorTests, BasicTransformation) {
    Rotor rotor(config);
    // Ensure initial position is 0
    rotor.setPosition(0);

    // Based on Rotor1.toml: 0 maps to 3
    EXPECT_EQ(rotor.transform(0, false), 3);

    // Reverse: 3 maps to 0
    EXPECT_EQ(rotor.transform(3, true), 0);
}

TEST_F(RotorTests, Reciprocity) {
    Rotor rotor(config);
    rotor.setPosition(0);

    // Verify that the Reverse LUT is the exact inverse of the Forward LUT
    // This explicitly tests the logic in initReverseTransformLUT()
    for (int i = 0; i < 26; i++) {
        int forward = rotor.transform(i, false);
        int reverse = rotor.transform(forward, true);
        EXPECT_EQ(reverse, i) << "Reciprocity failed at input " << i;
    }
}

TEST_F(RotorTests, RotationEffect) {
    Rotor rotor(config);
    rotor.setPosition(0);

    // Initial: 0 -> 3
    int initialOutput = rotor.transform(0, false);
    EXPECT_EQ(initialOutput, 3);

    // Rotate 1 step
    rotor.rotate();

    // After rotation, the mapping changes.
    // The exact new value depends on the internal math (Ring setting vs Position),
    // but it MUST be different from the initial for a standard rotor.
    int rotatedOutput = rotor.transform(0, false);
    EXPECT_NE(rotatedOutput, initialOutput);
}

TEST_F(RotorTests, FullRotationCycle) {
    Rotor rotor(config);
    rotor.setPosition(0);

    int startVal = rotor.transform(0, false);

    // Rotate 26 times to return to start
    for (int i = 0; i < 26; i++) {
        rotor.rotate();
    }

    EXPECT_EQ(rotor.transform(0, false), startVal);
}

TEST_F(RotorTests, SetPosition) {
    Rotor rotor(config);

    rotor.setPosition(5);
    int valAt5 = rotor.transform(0, false);

    rotor.setPosition(0);
    rotor.rotate();
    rotor.rotate();
    rotor.rotate();
    rotor.rotate();
    rotor.rotate();
    // Now effectively at 5

    EXPECT_EQ(rotor.transform(0, false), valAt5);
}

TEST_F(RotorTests, NotchSignaling) {
    Rotor rotor(config);

    // Rotor1 notch is at 0 (from config)
    // We want to step INTO the notch.
    // If we are at 25, next step is 0 (Notch).

    rotor.setPosition(25);
    int signal = rotor.rotate();  // Becomes 0
    EXPECT_EQ(signal, 1) << "Rotor should signal notch when stepping into position 0";

    // Step again (to 1)
    signal = rotor.rotate();
    EXPECT_EQ(signal, 0) << "Rotor should not signal notch when stepping out of 0";
}

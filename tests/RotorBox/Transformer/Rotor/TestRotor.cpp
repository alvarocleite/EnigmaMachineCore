#include <gtest/gtest.h>
#include "Rotor.hpp"

class RotorTests : public ::testing::Test {
protected:
    const std::string configPath = "assets/Rotor1.toml";
};

TEST_F(RotorTests, InitializationAndType) {
    Rotor rotor(configPath);
    EXPECT_EQ(rotor.getType(), TransformerType::Rotor);
}

TEST_F(RotorTests, BasicTransformation) {
    Rotor rotor(configPath);
    // Ensure initial position is 0
    rotor.setPosition(0);

    // Based on Rotor1.toml: 0 maps to 3
    EXPECT_EQ(rotor.transform(0, false), 3);
    
    // Reverse: 3 maps to 0
    EXPECT_EQ(rotor.transform(3, true), 0);
}

TEST_F(RotorTests, RotationEffect) {
    Rotor rotor(configPath);
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
    Rotor rotor(configPath);
    rotor.setPosition(0);
    
    int startVal = rotor.transform(0, false);
    
    // Rotate 26 times to return to start
    for(int i = 0; i < 26; i++) {
        rotor.rotate();
    }
    
    EXPECT_EQ(rotor.transform(0, false), startVal);
}

TEST_F(RotorTests, SetPosition) {
    Rotor rotor(configPath);
    
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
#include <gtest/gtest.h>
#include "Reflector.hpp"

class ReflectorTests : public ::testing::Test {
protected:
    // Path to the asset file copied by CMake
    const std::string configPath = "assets/Reflector.toml";
};

TEST_F(ReflectorTests, InitializationAndType) {
    Reflector reflector(configPath);
    EXPECT_EQ(reflector.getType(), TransformerType::Reflector);
}

TEST_F(ReflectorTests, ForwardTransformation) {
    Reflector reflector(configPath);
    
    // Based on Reflector.toml: 0 maps to 3
    EXPECT_EQ(reflector.transform(0), 3);
    
    // Based on Reflector.toml: 4 maps to 7
    EXPECT_EQ(reflector.transform(4), 7);
}

TEST_F(ReflectorTests, Reciprocity) {
    Reflector reflector(configPath);
    
    // If A -> D, then D -> A
    int input = 0;
    int output = reflector.transform(input);
    EXPECT_EQ(reflector.transform(output), input);

    // Test another pair
    input = 10; // K -> R (17) in standard, checking config...
    // In Reflector.toml: index 10 is 17
    output = reflector.transform(10);
    EXPECT_EQ(output, 17);
    EXPECT_EQ(reflector.transform(17), 10);
}

TEST_F(ReflectorTests, NoRotation) {
    Reflector reflector(configPath);
    
    // Reflectors are static, rotate() should return 0 (no carry)
    EXPECT_EQ(reflector.rotate(), 0);
    
    // Transform should remain the same after "rotate"
    int initial = reflector.transform(0);
    reflector.rotate();
    EXPECT_EQ(reflector.transform(0), initial);
}

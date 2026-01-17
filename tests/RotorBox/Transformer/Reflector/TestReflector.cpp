#include <gtest/gtest.h>
#include "EnigmaMachineConfig.hpp"
#include "EnigmaConfigLoader.hpp"
#include "FileAssetProvider.hpp"
#include "Reflector.hpp"

class ReflectorTests : public ::testing::Test {
protected:
    // Path to the asset file copied by CMake
    const std::string configPath = "assets/Reflector.toml";
    ReflectorConfig config;

    void SetUp() override {
        FileAssetProvider provider;
        config = EnigmaConfigLoader::loadReflector(provider, configPath);
    }
};

TEST_F(ReflectorTests, InitializationAndType) {
    Reflector reflector(config);
    EXPECT_EQ(reflector.getType(), TransformerType::Reflector);
}

TEST_F(ReflectorTests, ForwardTransformation) {
    Reflector reflector(config);

    // Based on Reflector.toml: 0 maps to 3
    EXPECT_EQ(reflector.transform(0), 3);

    // Based on Reflector.toml: 4 maps to 7
    EXPECT_EQ(reflector.transform(4), 7);
}

TEST_F(ReflectorTests, Reciprocity) {
    Reflector reflector(config);

    // If A -> D, then D -> A
    int input = 0;
    int output = reflector.transform(input);
    EXPECT_EQ(reflector.transform(output), input);

    // Test another pair
    input = 10;  // K -> R (17) in standard, checking config...
    // In Reflector.toml: index 10 is 17
    output = reflector.transform(10);
    EXPECT_EQ(output, 17);
    EXPECT_EQ(reflector.transform(17), 10);
}

TEST_F(ReflectorTests, NoRotation) {
    Reflector reflector(config);

    // Reflectors are static, rotate() should return 0 (no carry)
    EXPECT_EQ(reflector.rotate(), 0);

    // Transform should remain the same after "rotate"
    int initial = reflector.transform(0);
    reflector.rotate();
    EXPECT_EQ(reflector.transform(0), initial);
}
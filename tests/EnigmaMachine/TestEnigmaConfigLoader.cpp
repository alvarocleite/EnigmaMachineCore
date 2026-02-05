#include <gtest/gtest.h>
#include "EnigmaConfigLoader.hpp"
#include "EnigmaMachineConfig.hpp"
#include "FileAssetProvider.hpp"
#include "config.hpp"

using FileName = EnigmaConfigLoader::FileName;
using AssetPath = EnigmaConfigLoader::AssetPath;
class EnigmaConfigLoaderTests : public ::testing::Test {
protected:
    const std::string validConfigPath = "assets/EnigmaMachineConfig1.toml";
    const std::string invalidConfigPath = "assets/non_existent.toml";
};

TEST_F(EnigmaConfigLoaderTests, LoadValidConfig) {
    EnigmaMachineConfig config;
    FileAssetProvider provider;
    EXPECT_NO_THROW({ config = EnigmaConfigLoader::load(provider, FileName(validConfigPath), AssetPath(assetsDir)); });

    EXPECT_EQ(config.rotorCount, 3);

    std::vector<int> expectedPositions = {6, 18, 1};
    EXPECT_EQ(config.rotorPositions, expectedPositions);

    const auto& rotors = config.rotors;
    EXPECT_EQ(rotors.size(), 3);

    const auto& pairs = config.plugBoardPairs;
    // Check a few pairs
    // { from =  4, to =  7 }
    // { from = 18, to = 20 }
    bool foundFirst = false;
    bool foundSecond = false;

    // Note: The array size is PLUGBOARD_MAX_PAIRS. Unused are (-1, -1).
    for (const auto& p : pairs) {
        if (p.sourcePortIndex == 4 && p.destinationPortIndex == 7) foundFirst = true;
        if (p.sourcePortIndex == 18 && p.destinationPortIndex == 20) foundSecond = true;
    }
    EXPECT_TRUE(foundFirst);
    EXPECT_TRUE(foundSecond);
}

TEST_F(EnigmaConfigLoaderTests, LoadInvalidConfig) {
    FileAssetProvider provider;
    EXPECT_THROW(
        { EnigmaConfigLoader::load(provider, FileName(invalidConfigPath), AssetPath(assetsDir)); }, std::exception);
}

TEST_F(EnigmaConfigLoaderTests, RotorConfigProperties) {
    FileAssetProvider provider;
    EnigmaMachineConfig config = EnigmaConfigLoader::load(provider, FileName(validConfigPath), AssetPath(assetsDir));
    const auto& rotors = config.rotors;
    ASSERT_FALSE(rotors.empty());

    // Check properties of the first loaded rotor (Rotor1)
    // We expect size 26
    EXPECT_EQ(rotors[0].wiring.size(), 26);
    EXPECT_GE(rotors[0].notchPosition, 0);
    EXPECT_LT(rotors[0].notchPosition, 26);
}

// Mock implementation to test error handling for malformed content
class MalformedAssetProvider : public IAssetProvider {
public:
    std::string loadAsset(std::string_view assetName) override {
        if (assetName == "bad_rotor.toml") {
            // Missing 'forward' wiring array
            return "[rotor]\nnotchPosition = 0\ntype = \"rotor\"\nsize = 26";
        }
        if (assetName == "wrong_type.toml") {
            return "[rotor]\nnotchPosition = 0\ntype = \"reflector\"\nsize = 26";
        }
        if (assetName == "inconsistent_count.toml") {
            // RotorCount is 3, but RotorFiles has 2
            return "[rotors]\nRotorCount = 3\nRotorPositions = [0, 0, 0]\nRotorFiles = [\"R1.toml\", \"R2.toml\"]";
        }
        throw std::runtime_error("File not found");
    }
};

TEST_F(EnigmaConfigLoaderTests, LoadMalformedRotor) {
    MalformedAssetProvider provider;
    // Should throw because 'forward' is missing or validation fails
    EXPECT_THROW({ EnigmaConfigLoader::loadRotor(provider, FileName("bad_rotor.toml")); }, std::exception);
}

TEST_F(EnigmaConfigLoaderTests, LoadWrongComponentType) {
    MalformedAssetProvider provider;
    // Should throw because type is 'reflector' but we expect 'rotor'
    EXPECT_THROW({ EnigmaConfigLoader::loadRotor(provider, FileName("wrong_type.toml")); }, std::exception);
}

TEST_F(EnigmaConfigLoaderTests, LoadInconsistentConfig) {
    MalformedAssetProvider provider;
    // Should throw because count mismatch
    EXPECT_THROW({ EnigmaConfigLoader::load(provider, FileName("inconsistent_count.toml")); }, std::exception);
}

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

/** @brief Verifies loading a valid configuration file. */
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
    bool foundFirst = false;
    bool foundSecond = false;

    for (const auto& p : pairs) {
        if (p.sourcePortIndex == 4 && p.destinationPortIndex == 7) foundFirst = true;
        if (p.sourcePortIndex == 18 && p.destinationPortIndex == 20) foundSecond = true;
    }
    EXPECT_TRUE(foundFirst);
    EXPECT_TRUE(foundSecond);
}

/** @brief Verifies exception thrown for non-existent config file. */
TEST_F(EnigmaConfigLoaderTests, LoadInvalidConfig) {
    FileAssetProvider provider;
    EXPECT_THROW(
        { EnigmaConfigLoader::load(provider, FileName(invalidConfigPath), AssetPath(assetsDir)); }, std::exception);
}

/** @brief Verifies rotor configuration properties are correctly loaded. */
TEST_F(EnigmaConfigLoaderTests, RotorConfigProperties) {
    FileAssetProvider provider;
    EnigmaMachineConfig config = EnigmaConfigLoader::load(provider, FileName(validConfigPath), AssetPath(assetsDir));
    const auto& rotors = config.rotors;
    ASSERT_FALSE(rotors.empty());

    EXPECT_EQ(rotors[0].wiring.size(), 26);
    EXPECT_GE(rotors[0].notchPosition, 0);
    EXPECT_LT(rotors[0].notchPosition, 26);
}

/** @brief Mock provider for testing error handling with malformed content. */
class MalformedAssetProvider : public IAssetProvider {
public:
    std::string loadAsset(std::string_view assetName) const override {
        if (assetName == "bad_rotor.toml") {
            return "[rotor]\nnotchPosition = 0\ntype = \"rotor\"\nsize = 26";
        }
        if (assetName == "wrong_type.toml") {
            return "[rotor]\nnotchPosition = 0\ntype = \"reflector\"\nsize = 26";
        }
        if (assetName == "inconsistent_count.toml") {
            return "[rotors]\nRotorCount = 3\nRotorPositions = [0, 0, 0]\nRotorFiles = [\"R1.toml\", \"R2.toml\"]";
        }
        throw std::runtime_error("File not found");
    }
};

/** @brief Verifies exception thrown for malformed rotor configuration. */
TEST_F(EnigmaConfigLoaderTests, LoadMalformedRotor) {
    MalformedAssetProvider provider;
    EXPECT_THROW({ EnigmaConfigLoader::loadRotor(provider, FileName("bad_rotor.toml")); }, std::exception);
}

/** @brief Verifies exception thrown when loading wrong component type. */
TEST_F(EnigmaConfigLoaderTests, LoadWrongComponentType) {
    MalformedAssetProvider provider;
    EXPECT_THROW({ EnigmaConfigLoader::loadRotor(provider, FileName("wrong_type.toml")); }, std::exception);
}

/** @brief Verifies exception thrown for inconsistent configuration. */
TEST_F(EnigmaConfigLoaderTests, LoadInconsistentConfig) {
    MalformedAssetProvider provider;
    EXPECT_THROW({ EnigmaConfigLoader::load(provider, FileName("inconsistent_count.toml")); }, std::exception);
}

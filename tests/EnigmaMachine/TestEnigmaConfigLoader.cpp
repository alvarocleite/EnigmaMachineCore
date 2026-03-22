#include <gtest/gtest.h>
#include "EnigmaConfig.hpp"
#include "EnigmaConfigLoader.hpp"
#include "EnigmaError.hpp"
#include "EnigmaMachineConfig.hpp"
#include "FileAssetProvider.hpp"

using FileName = EnigmaConfigLoader::FileName;
using AssetPath = EnigmaConfigLoader::AssetPath;
class EnigmaConfigLoaderTests : public ::testing::Test {
protected:
    const std::string validConfigPath = "assets/EnigmaMachineConfig1.toml";
    const std::string invalidConfigPath = "assets/non_existent.toml";
};

/** @brief Verifies loading a valid configuration file. */
TEST_F(EnigmaConfigLoaderTests, LoadValidConfig) {
    FileAssetProvider provider;
    auto result = EnigmaConfigLoader::load(provider, FileName(validConfigPath), AssetPath(enigma::assetsDir));
    ASSERT_TRUE(result.has_value());

    const auto& config = *result;
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

/** @brief Verifies error returned for non-existent config file. */
TEST_F(EnigmaConfigLoaderTests, LoadInvalidConfig) {
    FileAssetProvider provider;
    auto result = EnigmaConfigLoader::load(provider, FileName(invalidConfigPath), AssetPath(enigma::assetsDir));
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), enigma::EnigmaError::FileNotFound);
}

/** @brief Verifies rotor configuration properties are correctly loaded. */
TEST_F(EnigmaConfigLoaderTests, RotorConfigProperties) {
    FileAssetProvider provider;
    auto result = EnigmaConfigLoader::load(provider, FileName(validConfigPath), AssetPath(enigma::assetsDir));
    ASSERT_TRUE(result.has_value());

    const auto& rotors = result->rotors;
    ASSERT_FALSE(rotors.empty());

    EXPECT_EQ(rotors[0].wiring.size(), 26);
    EXPECT_GE(rotors[0].notchPosition, 0);
    EXPECT_LT(rotors[0].notchPosition, 26);
}

/** @brief Mock provider for testing error handling with malformed content. */
class MalformedAssetProvider : public IAssetProvider {
public:
    enigma::Result<std::string> loadAsset(std::string_view assetName) const override {
        if (assetName == "bad_rotor.toml") {
            return std::string(
                "[rotor]\nnotchPosition = 0\ntype = \"rotor\"\nsize = 26\nforward = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, "
                "11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24]");
        }
        if (assetName == "wrong_type.toml") {
            return std::string("[rotor]\nnotchPosition = 0\ntype = \"reflector\"\nsize = 26");
        }
        if (assetName == "inconsistent_count.toml") {
            return std::string(
                "[rotors]\nRotorCount = 3\nRotorPositions = [0, 0, 0]\nRotorFiles = [\"R1.toml\", \"R2.toml\"]");
        }
        return nonstd::make_unexpected(enigma::EnigmaError::FileNotFound);
    }
};

/** @brief Verifies error returned for malformed rotor configuration. */
TEST_F(EnigmaConfigLoaderTests, LoadMalformedRotor) {
    MalformedAssetProvider provider;
    auto result = EnigmaConfigLoader::loadRotor(provider, FileName("bad_rotor.toml"));
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), enigma::EnigmaError::ConfigFieldMissing);
}

/** @brief Verifies error returned when rotor configuration is missing required fields. */
TEST_F(EnigmaConfigLoaderTests, LoadMissingRotorField) {
    MalformedAssetProvider provider;
    auto result = EnigmaConfigLoader::loadRotor(provider, FileName("wrong_type.toml"));
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), enigma::EnigmaError::ConfigFieldMissing);
}

/** @brief Verifies error returned for inconsistent configuration. */
TEST_F(EnigmaConfigLoaderTests, LoadInconsistentConfig) {
    MalformedAssetProvider provider;
    auto result = EnigmaConfigLoader::load(provider, FileName("inconsistent_count.toml"));
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), enigma::EnigmaError::ConfigCountMismatch);
}

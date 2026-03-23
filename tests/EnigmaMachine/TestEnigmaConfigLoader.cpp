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
        if (assetName == "wrong_size.toml") {
            return std::string(
                "[rotor]\nnotchPosition = 0\ntype = \"rotor\"\nsize = 24\nforward = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, "
                "11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25]");
        }
        if (assetName == "R1.toml") {
            return std::string(
                "[rotor]\nnotchPosition = 0\ntype = \"rotor\"\nsize = 26\nforward = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, "
                "11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25]");
        }
        if (assetName == "Reflector.toml") {
            return std::string(
                "[reflector]\ntype = \"reflector\"\nsize = 26\nmap = [24, 17, 20, 7, 16, 22, 15, 23, 18, 25, "
                "8, 13, 1, 11, 4, 2, 19, 12, 14, 21, 6, 9, 3, 0, 10, 5]");
        }
        if (assetName == "too_many_plugs.toml") {
            return std::string(
                "[rotors]\nRotorCount = 1\nRotorPositions = [0]\nRotorFiles = [\"valid_rotor.toml\"]\n"
                "ReflectorFile = \"valid_reflector.toml\"\n"
                "[plugboard]\nPlugCount = 15\nPlugBoardPairs = [{from = 0, to = 1}, {from = 2, to = 3}, {from = 4, to "
                "= 5}, "
                "{from = 6, to = 7}, {from = 8, to = 9}, {from = 10, to = 11}, {from = 12, to = 13}, "
                "{from = 14, to = 15}, {from = 16, to = 17}, {from = 18, to = 19}, {from = 20, to = 21}, "
                "{from = 22, to = 23}, {from = 24, to = 25}, {from = 3, to = 0}, {from = 5, to = 2}]");
        }
        if (assetName == "mismatched_plug_count.toml") {
            return std::string(
                "[rotors]\nRotorCount = 1\nRotorPositions = [0]\nRotorFiles = [\"valid_rotor.toml\"]\n"
                "ReflectorFile = \"valid_reflector.toml\"\n"
                "[plugboard]\nPlugCount = 3\nPlugBoardPairs = [{from = 0, to = 1}, {from = 2, to = 3}]");
        }
        if (assetName == "valid_rotor.toml") {
            return std::string(
                "[rotor]\nnotchPosition = 0\ntype = \"rotor\"\nsize = 26\nforward = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, "
                "11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25]");
        }
        if (assetName == "valid_reflector.toml") {
            return std::string(
                "[reflector]\ntype = \"reflector\"\nsize = 26\nmap = [24, 17, 20, 7, 16, 22, 15, 23, 18, 25, "
                "8, 13, 1, 11, 4, 2, 19, 12, 14, 21, 6, 9, 3, 0, 10, 5]");
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

/** @brief Verifies error returned when rotor has wrong size. */
TEST_F(EnigmaConfigLoaderTests, LoadRotorWrongSize) {
    MalformedAssetProvider provider;
    auto result = EnigmaConfigLoader::loadRotor(provider, FileName("wrong_size.toml"));
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), enigma::EnigmaError::ConfigFieldMissing);
}

/** @brief Verifies error returned when plugboard exceeds maximum pairs. */
TEST_F(EnigmaConfigLoaderTests, LoadConfigTooManyPlugs) {
    MalformedAssetProvider provider;
    auto result = EnigmaConfigLoader::load(provider, FileName("too_many_plugs.toml"));
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), enigma::EnigmaError::ConfigFieldMissing);
}

/** @brief Verifies error returned when plugboard count doesn't match array size. */
TEST_F(EnigmaConfigLoaderTests, LoadConfigMismatchedPlugCount) {
    MalformedAssetProvider provider;
    auto result = EnigmaConfigLoader::load(provider, FileName("mismatched_plug_count.toml"));
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), enigma::EnigmaError::ConfigFieldMissing);
}

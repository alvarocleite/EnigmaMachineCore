#include <gtest/gtest.h>
#include <fstream>
#include "EnigmaConfig.hpp"
#include "EnigmaError.hpp"
#include "FileAssetProvider.hpp"

class FileAssetProviderTests : public ::testing::Test {
protected:
    FileAssetProvider provider;
    const std::string existingAsset = std::string(enigma::assetsDir) + "Rotor1.toml";
    const std::string nonExistentAsset = std::string(enigma::assetsDir) + "GhostRotor.toml";
};

/** @brief Verifies loading an existing asset file. */
TEST_F(FileAssetProviderTests, LoadExistingFile) {
    auto result = provider.loadAsset(existingAsset);
    EXPECT_TRUE(result.has_value());
    EXPECT_FALSE(result->empty());
    EXPECT_NE(result->find("[rotor]"), std::string::npos);
}

/** @brief Verifies error returned for non-existent asset. */
TEST_F(FileAssetProviderTests, LoadNonExistentFile) {
    auto result = provider.loadAsset(nonExistentAsset);
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), enigma::EnigmaError::FileNotFound);
}

/** @brief Mock provider for verifying IAssetProvider interface. */
class MockAssetProvider : public IAssetProvider {
public:
    enigma::Result<std::string> loadAsset(std::string_view assetName) const override {
        if (assetName == "mock_rotor") {
            return std::string("[rotor]\nnotchPosition = 0\nforward = [0, 1, 2]");
        }
        return nonstd::make_unexpected(enigma::EnigmaError::FileNotFound);
    }
};

/** @brief Verifies IAssetProvider interface contract with mock implementation. */
TEST(IAssetProviderTests, MockImplementation) {
    MockAssetProvider mock;
    auto result = mock.loadAsset("mock_rotor");
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(*result, "[rotor]\nnotchPosition = 0\nforward = [0, 1, 2]");

    auto failResult = mock.loadAsset("unknown");
    EXPECT_FALSE(failResult.has_value());
    EXPECT_EQ(failResult.error(), enigma::EnigmaError::FileNotFound);
}

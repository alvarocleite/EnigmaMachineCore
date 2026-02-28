#include <gtest/gtest.h>
#include <fstream>
#include "FileAssetProvider.hpp"
#include "config.hpp"

class FileAssetProviderTests : public ::testing::Test {
protected:
    FileAssetProvider provider;
    const std::string existingAsset = std::string(assetsDir) + "Rotor1.toml";
    const std::string nonExistentAsset = std::string(assetsDir) + "GhostRotor.toml";
};

TEST_F(FileAssetProviderTests, LoadExistingFile) {
    std::string content;
    EXPECT_NO_THROW({ content = provider.loadAsset(existingAsset); });
    EXPECT_FALSE(content.empty());

    // Check for known TOML content (Rotor1.toml should contain "[rotor]")
    EXPECT_NE(content.find("[rotor]"), std::string::npos);
}

TEST_F(FileAssetProviderTests, LoadNonExistentFile) {
    EXPECT_THROW({ provider.loadAsset(nonExistentAsset); }, std::runtime_error);
}

// Mock implementation to verify Interface usage
class MockAssetProvider : public IAssetProvider {
public:
    std::string loadAsset(std::string_view assetName) const override {
        if (assetName == "mock_rotor") {
            return "[rotor]\nnotchPosition = 0\nforward = [0, 1, 2]";
        }
        throw std::runtime_error("Mock file not found");
    }
};

TEST(IAssetProviderTests, MockImplementation) {
    MockAssetProvider mock;
    std::string content = mock.loadAsset("mock_rotor");
    EXPECT_EQ(content, "[rotor]\nnotchPosition = 0\nforward = [0, 1, 2]");

    EXPECT_THROW(mock.loadAsset("unknown"), std::runtime_error);
}

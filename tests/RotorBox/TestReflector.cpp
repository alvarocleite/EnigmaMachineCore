#include <gtest/gtest.h>
#include "EnigmaConfigLoader.hpp"
#include "EnigmaMachineConfig.hpp"
#include "FileAssetProvider.hpp"
#include "Reflector.hpp"

using FileName = EnigmaConfigLoader::FileName;
class ReflectorTests : public ::testing::Test {
protected:
    static constexpr std::string_view configPath = "assets/Reflector.toml";
    ReflectorConfig config;

    void SetUp() override {
        FileAssetProvider provider;
        auto result = EnigmaConfigLoader::loadReflector(provider, FileName(configPath));
        ASSERT_TRUE(result.has_value());
        config = *result;
    }
};

/** @brief Verifies reflector initialization and type identification. */
TEST_F(ReflectorTests, InitializationAndType) {
    Reflector reflector(config);
    EXPECT_EQ(reflector.getType(), TransformerType::Reflector);
}

/** @brief Verifies forward signal transformation through reflector. */
TEST_F(ReflectorTests, ForwardTransformation) {
    Reflector reflector(config);

    EXPECT_EQ(reflector.transform(0), 3);
    EXPECT_EQ(reflector.transform(4), 7);
}

/** @brief Verifies reflector is reciprocal: transform(transform(x)) == x. */
TEST_F(ReflectorTests, Reciprocity) {
    Reflector reflector(config);

    int input = 0;
    int output = reflector.transform(input);
    EXPECT_EQ(reflector.transform(output), input);

    input = 10;
    output = reflector.transform(10);
    EXPECT_EQ(output, 17);
    EXPECT_EQ(reflector.transform(17), 10);
}

/** @brief Verifies reflectors do not rotate (static components). */
TEST_F(ReflectorTests, NoRotation) {
    Reflector reflector(config);

    EXPECT_EQ(reflector.rotate(), 0);

    int initial = reflector.transform(0);
    reflector.rotate();
    EXPECT_EQ(reflector.transform(0), initial);
}

/** @brief Verifies move constructor transfers state correctly. */
TEST_F(ReflectorTests, MoveConstructor) {
    ReflectorConfig configCopy = config;
    Reflector reflector(std::move(configCopy));

    EXPECT_EQ(reflector.getType(), TransformerType::Reflector);
    EXPECT_EQ(reflector.transform(0), 3);
}

/** @brief Verifies move constructor produces same results as copy constructor. */
TEST_F(ReflectorTests, MoveConstructorResultsMatchCopy) {
    ReflectorConfig configCopy = config;
    Reflector reflectorCopy(configCopy);

    ReflectorConfig configMove = config;
    Reflector reflectorMove(std::move(configMove));

    for (int i = 0; i < 26; i++) {
        EXPECT_EQ(reflectorMove.transform(i), reflectorCopy.transform(i));
    }
}

/** @brief Verifies reverse transformation returns -1 (reflectors are unidirectional). */
TEST_F(ReflectorTests, ReverseTransformation) {
    Reflector reflector(config);

    EXPECT_EQ(reflector.transform(0, true), -1);
    EXPECT_EQ(reflector.transform(10, true), -1);
    EXPECT_EQ(reflector.transform(25, true), -1);
}

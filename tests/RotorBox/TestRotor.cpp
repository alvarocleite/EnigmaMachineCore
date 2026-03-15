#include <gtest/gtest.h>
#include <array>
#include "EnigmaConfigLoader.hpp"
#include "EnigmaMachineConfig.hpp"
#include "FileAssetProvider.hpp"
#include "Rotor.hpp"

class RotorTests : public ::testing::Test {
protected:
    static constexpr std::string_view configPath = "assets/Rotor1.toml";
    RotorConfig config;

    void SetUp() override {
        FileAssetProvider provider;
        config = EnigmaConfigLoader::loadRotor(provider, EnigmaConfigLoader::FileName(configPath));
    }
};

/** @brief Verifies rotor initialization and type identification. */
TEST_F(RotorTests, InitializationAndType) {
    Rotor rotor(config);
    EXPECT_EQ(rotor.getType(), TransformerType::Rotor);
}

/** @brief Verifies basic forward and reverse transformation. */
TEST_F(RotorTests, BasicTransformation) {
    Rotor rotor(config);
    rotor.setPosition(0);

    EXPECT_EQ(rotor.transform(0, false), 3);
    EXPECT_EQ(rotor.transform(3, true), 0);
}

/** @brief Verifies rotor is reciprocal: transform(reverse(transform(x))) == x. */
TEST_F(RotorTests, Reciprocity) {
    Rotor rotor(config);
    rotor.setPosition(0);

    for (int i = 0; i < 26; i++) {
        int forward = rotor.transform(i, false);
        int reverse = rotor.transform(forward, true);
        EXPECT_EQ(reverse, i) << "Reciprocity failed at input " << i;
    }
}

/** @brief Verifies rotation changes the transformation mapping. */
TEST_F(RotorTests, RotationEffect) {
    Rotor rotor(config);
    rotor.setPosition(0);

    int initialOutput = rotor.transform(0, false);
    EXPECT_EQ(initialOutput, 3);

    rotor.rotate();

    int rotatedOutput = rotor.transform(0, false);
    EXPECT_NE(rotatedOutput, initialOutput);
}

/** @brief Verifies 26 rotations return to starting position. */
TEST_F(RotorTests, FullRotationCycle) {
    Rotor rotor(config);
    rotor.setPosition(0);

    int startVal = rotor.transform(0, false);

    for (int i = 0; i < 26; i++) {
        rotor.rotate();
    }

    EXPECT_EQ(rotor.transform(0, false), startVal);
}

/** @brief Verifies setPosition manually sets rotor position. */
TEST_F(RotorTests, SetPosition) {
    Rotor rotor(config);

    rotor.setPosition(5);
    int valAt5 = rotor.transform(0, false);

    rotor.setPosition(0);
    rotor.rotate();
    rotor.rotate();
    rotor.rotate();
    rotor.rotate();
    rotor.rotate();

    EXPECT_EQ(rotor.transform(0, false), valAt5);
}

/** @brief Verifies notch signaling when rotor steps into notch position. */
TEST_F(RotorTests, NotchSignaling) {
    Rotor rotor(config);

    rotor.setPosition(25);
    int signal = rotor.rotate();
    EXPECT_EQ(signal, 1) << "Rotor should signal notch when stepping into position 0";

    signal = rotor.rotate();
    EXPECT_EQ(signal, 0) << "Rotor should not signal notch when stepping out of 0";
}

/** @brief Verifies move constructor transfers state correctly. */
TEST_F(RotorTests, MoveConstructor) {
    RotorConfig configCopy = config;
    Rotor rotor(std::move(configCopy));

    EXPECT_EQ(rotor.getType(), TransformerType::Rotor);
    EXPECT_EQ(rotor.transform(0, false), 3);
    EXPECT_EQ(rotor.transform(3, true), 0);
}

/** @brief Verifies reverse transform handles wrap-around at boundaries. */
TEST_F(RotorTests, ReverseTransformWrapAround) {
    Rotor rotor(config);

    for (int rot = 0; rot < 26; rot++) {
        rotor.setPosition(rot);
        for (int i = 0; i < 26; i++) {
            int result = rotor.transform(i, true);
            EXPECT_GE(result, 0);
            EXPECT_LT(result, 26);
        }
    }

    rotor.setPosition(10);
    int res1 = rotor.transform(0, true);
    rotor.setPosition(0);
    int res2 = rotor.transform(10, true);
    EXPECT_NE(res1, res2) << "Different rotation positions should produce different results";
}

/** @brief Verifies move constructor produces same results as copy. */
TEST_F(RotorTests, MoveConstructorResultsMatchCopy) {
    RotorConfig configCopy = config;
    Rotor rotorCopy(configCopy);
    rotorCopy.setPosition(0);

    RotorConfig configMove = config;
    Rotor rotorMove(std::move(configMove));
    rotorMove.setPosition(0);

    for (int i = 0; i < 26; i++) {
        EXPECT_EQ(rotorMove.transform(i, false), rotorCopy.transform(i, false));
        EXPECT_EQ(rotorMove.transform(i, true), rotorCopy.transform(i, true));
    }
}

/** @brief Verifies exception thrown for wiring value out of range. */
TEST(RotorErrorTests, InvalidWiringValueOutOfRange) {
    RotorConfig invalidConfig;
    invalidConfig.wiring = std::array<int, 26>{0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12,
                                               13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 30};
    invalidConfig.notchPosition = 0;

    EXPECT_THROW(Rotor rotor(invalidConfig), std::runtime_error);
}

/** @brief Verifies exception thrown for duplicate wiring values. */
TEST(RotorErrorTests, InvalidWiringDuplicateValue) {
    RotorConfig invalidConfig;
    invalidConfig.wiring = std::array<int, 26>{0,  0,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12,
                                               13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25};
    invalidConfig.notchPosition = 0;

    EXPECT_THROW(Rotor rotor(invalidConfig), std::runtime_error);
}

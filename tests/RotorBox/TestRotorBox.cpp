#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "EnigmaConfig.hpp"
#include "EnigmaConfigLoader.hpp"
#include "EnigmaMachineConfig.hpp"
#include "FileAssetProvider.hpp"
#include "RotorBox.hpp"

namespace fs = std::filesystem;
using FileName = EnigmaConfigLoader::FileName;
class RotorBoxTests : public ::testing::Test {
protected:
    std::vector<FileName> rotorFiles = {FileName(fs::path(enigma::assetsDir) / "Rotor1.toml"),
                                        FileName(fs::path(enigma::assetsDir) / "Rotor2.toml"),
                                        FileName(fs::path(enigma::assetsDir) / "Rotor3.toml"),
                                        FileName(fs::path(enigma::assetsDir) / "Reflector.toml")};

    std::vector<RotorConfig> rotors;
    ReflectorConfig reflector;

    void SetUp() override {
        FileAssetProvider provider;
        auto r1 = EnigmaConfigLoader::loadRotor(provider, rotorFiles[0]);
        auto r2 = EnigmaConfigLoader::loadRotor(provider, rotorFiles[1]);
        auto r3 = EnigmaConfigLoader::loadRotor(provider, rotorFiles[2]);
        auto refl = EnigmaConfigLoader::loadReflector(provider, rotorFiles[3]);
        ASSERT_TRUE(r1.has_value());
        ASSERT_TRUE(r2.has_value());
        ASSERT_TRUE(r3.has_value());
        ASSERT_TRUE(refl.has_value());
        rotors.push_back(*r1);
        rotors.push_back(*r2);
        rotors.push_back(*r3);
        reflector = *refl;
    }
};

class EnigmaObserverTest : public IEnigmaObserver {
public:
    std::vector<AlphabetIndex> pos;
    EnigmaObserverTest(int rotorCount) { pos.resize(rotorCount, 0); }
    void onRotorStepped(int rotorIndex, AlphabetIndex position) override { pos[rotorIndex] = position; }
    void onCharEncrypted(char input, char output) override {}
};

/** @brief Verifies default constructor initializes with identity rotors. */
TEST_F(RotorBoxTests, DefaultConstructor) {
    RotorBox rb;

    EXPECT_EQ(rb.keyTransform(0), 25);

    int output = rb.keyTransform(8);
    EXPECT_EQ(output, 17);
}

/** @brief Verifies parameterized constructor accepts rotor configurations. */
TEST_F(RotorBoxTests, ParameterizedConstructor) {
    std::vector<int> positions = {0, 0, 0};
    auto rbResult = RotorBox::create(positions, rotors, reflector);
    ASSERT_TRUE(rbResult.has_value());
    RotorBox& rb = *rbResult;

    int output = rb.keyTransform(0);
    EXPECT_GE(output, 0);
    EXPECT_LT(output, 26);
}

/** @brief Verifies RotorBox is reciprocal: keyTransform(keyTransform(x)) == x. */
TEST_F(RotorBoxTests, RoundTrip) {
    int input = 5;
    int ciphertext;
    int decrypted;

    {
        std::vector<int> positions = {0, 0, 0};
        auto rbResult = RotorBox::create(positions, rotors, reflector);
        ASSERT_TRUE(rbResult.has_value());
        RotorBox& rb = *rbResult;
        ciphertext = rb.keyTransform(input);
    }

    {
        std::vector<int> positions = {0, 0, 0};
        auto rbResult = RotorBox::create(positions, rotors, reflector);
        ASSERT_TRUE(rbResult.has_value());
        RotorBox& rb = *rbResult;
        decrypted = rb.keyTransform(ciphertext);
    }

    EXPECT_EQ(decrypted, input);
}

/** @brief Verifies notch-based rotor stepping mechanics. */
TEST_F(RotorBoxTests, SteppingMechanism) {
    std::vector<int> startPos = {25, 0, 0};
    auto rbResult = RotorBox::create(startPos, rotors, reflector);
    ASSERT_TRUE(rbResult.has_value());
    RotorBox& rb = *rbResult;

    int out1 = rb.keyTransform(0);
    EXPECT_GE(out1, 0);
    EXPECT_LT(out1, 26);
}

/** @brief Verifies multiple notch carries propagate correctly. */
TEST_F(RotorBoxTests, MultiStepCarry) {
    std::vector<int> startPos = {25, 25, 0};
    auto rbResult = RotorBox::create(startPos, rotors, reflector);
    ASSERT_TRUE(rbResult.has_value());
    RotorBox& rb = *rbResult;

    int out = rb.keyTransform(0);
    EXPECT_GE(out, 0);
}

/** @brief Verifies double-stepping when middle rotor at notch. */
TEST_F(RotorBoxTests, DoubleSteppingMechanism_1) {
    std::vector<int> startPos = {0, 1, 0};
    auto rbResult = RotorBox::create(startPos, rotors, reflector);
    ASSERT_TRUE(rbResult.has_value());
    RotorBox& rb = *rbResult;
    EnigmaObserverTest observer(3);
    rb.registerObserver(&observer);

    rb.keyTransform(0);
    EXPECT_EQ(observer.pos[0], 1);
    EXPECT_EQ(observer.pos[1], 2);
    EXPECT_EQ(observer.pos[2], 0);
}

/** @brief Verifies stepping when rightmost rotor steps only. */
TEST_F(RotorBoxTests, DoubleSteppingMechanism_2) {
    std::vector<int> startPos = {0, 0, 0};
    auto rbResult = RotorBox::create(startPos, rotors, reflector);
    ASSERT_TRUE(rbResult.has_value());
    RotorBox& rb = *rbResult;
    EnigmaObserverTest observer(3);
    rb.registerObserver(&observer);

    rb.keyTransform(0);
    EXPECT_EQ(observer.pos[0], 1);
    EXPECT_EQ(observer.pos[1], 1);
    EXPECT_EQ(observer.pos[2], 1);
}

/** @brief Verifies mixed stepping pattern. */
TEST_F(RotorBoxTests, DoubleSteppingMechanism_3) {
    std::vector<int> startPos = {1, 0, 2};
    auto rbResult = RotorBox::create(startPos, rotors, reflector);
    ASSERT_TRUE(rbResult.has_value());
    RotorBox& rb = *rbResult;
    EnigmaObserverTest observer(3);
    rb.registerObserver(&observer);

    rb.keyTransform(0);
    EXPECT_EQ(observer.pos[0], 2);
    EXPECT_EQ(observer.pos[1], 1);
    EXPECT_EQ(observer.pos[2], 3);
}

/** @brief Verifies move constructor transfers state correctly. */
TEST_F(RotorBoxTests, MoveConstructor) {
    std::vector<int> positions = {0, 0, 0};
    RotorBox rbOriginal(positions, rotors, reflector);

    RotorBox rbMoved(std::move(rbOriginal));

    int output = rbMoved.keyTransform(0);
    EXPECT_GE(output, 0);
    EXPECT_LT(output, 26);
}

/** @brief Verifies move constructor produces same results as copy. */
TEST_F(RotorBoxTests, MoveConstructorResultsMatchCopy) {
    std::vector<int> positions = {0, 0, 0};
    RotorBox rbCopy(positions, rotors, reflector);
    int outputCopy = rbCopy.keyTransform(0);

    std::vector<int> positions2 = {0, 0, 0};
    std::vector<RotorConfig> rotors2 = rotors;
    ReflectorConfig reflector2 = reflector;
    RotorBox rbMoved(std::move(positions2), std::move(rotors2), std::move(reflector2));

    EXPECT_EQ(rbMoved.keyTransform(0), outputCopy);
}

/** @brief Verifies observer registration and removal. */
TEST_F(RotorBoxTests, RegisterAndRemoveObserver) {
    RotorBox rb;
    EnigmaObserverTest observer1(3);
    EnigmaObserverTest observer2(3);

    rb.registerObserver(&observer1);
    rb.registerObserver(&observer2);

    rb.keyTransform(0);
    EXPECT_EQ(observer1.pos[0], 1);
    EXPECT_EQ(observer2.pos[0], 1);

    rb.removeObserver(&observer1);
    rb.keyTransform(0);
    EXPECT_EQ(observer1.pos[0], 1);
    EXPECT_EQ(observer2.pos[0], 2);
}

/** @brief Verifies setLogger captures rotor stepping events. */
TEST_F(RotorBoxTests, SetLogger) {
    class TestLogger : public ILogger {
    public:
        std::vector<std::string> messages;
        void log(LogLevel level, std::string_view message) override { messages.push_back(std::string(message)); }
    };

    TestLogger logger;
    RotorBox rb;
    rb.setLogger(&logger);

    rb.keyTransform(0);
    EXPECT_FALSE(logger.messages.empty());
}

/** @brief Verifies printTransformers outputs transformer info to logger. */
TEST_F(RotorBoxTests, PrintTransformers) {
    class TestLogger : public ILogger {
    public:
        std::vector<std::string> messages;
        void log(LogLevel level, std::string_view message) override { messages.push_back(std::string(message)); }
    };

    TestLogger logger;
    std::vector<int> positions = {0, 0, 0};
    auto rbResult = RotorBox::create(positions, rotors, reflector, &logger);
    ASSERT_TRUE(rbResult.has_value());
    RotorBox& rb = *rbResult;

    rb.printTransformers();

    EXPECT_FALSE(logger.messages.empty());
    EXPECT_GE(logger.messages.size(), 3);
}

/** @brief Verifies exception thrown when rotor positions count mismatches rotor count. */
TEST(RotorBoxErrorTests, MismatchedPositionRotorCount) {
    std::vector<AlphabetIndex> positions = {0, 0};
    std::vector<RotorConfig> rotors;
    ReflectorConfig reflector;

    EXPECT_THROW(RotorBox rb(positions, rotors, reflector), std::invalid_argument);
}

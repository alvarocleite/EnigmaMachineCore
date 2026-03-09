#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "EnigmaConfigLoader.hpp"
#include "EnigmaMachineConfig.hpp"
#include "FileAssetProvider.hpp"
#include "RotorBox.hpp"
#include "config.hpp"

namespace fs = std::filesystem;
using FileName = EnigmaConfigLoader::FileName;
class RotorBoxTests : public ::testing::Test {
protected:
    std::vector<FileName> rotorFiles = {
        FileName(fs::path(assetsDir) / "Rotor1.toml"), FileName(fs::path(assetsDir) / "Rotor2.toml"),
        FileName(fs::path(assetsDir) / "Rotor3.toml"), FileName(fs::path(assetsDir) / "Reflector.toml")};

    std::vector<RotorConfig> rotors;
    ReflectorConfig reflector;

    void SetUp() override {
        FileAssetProvider provider;
        rotors.push_back(EnigmaConfigLoader::loadRotor(provider, rotorFiles[0]));
        rotors.push_back(EnigmaConfigLoader::loadRotor(provider, rotorFiles[1]));
        rotors.push_back(EnigmaConfigLoader::loadRotor(provider, rotorFiles[2]));
        reflector = EnigmaConfigLoader::loadReflector(provider, rotorFiles[3]);
    }
};

class EnigmaObserverTest : public IEnigmaObserver {
public:
    std::vector<AlphabetIndex> pos;
    EnigmaObserverTest(int rotorCount) { pos.resize(rotorCount, 0); }
    void onRotorStepped(int rotorIndex, AlphabetIndex position) override { pos[rotorIndex] = position; }
    void onCharEncrypted(char input, char output) override {
        // Not used in this test however needs to override as IEnigmaObserver is pure virtual
    }
};

TEST_F(RotorBoxTests, DefaultConstructor) {
    // Default constructor initializes 3 default rotors and 1 default reflector.
    RotorBox rb;

    // With default identity rotors and default reverse reflector map, 0 -> 25.
    EXPECT_EQ(rb.keyTransform(0), 25);

    int output = rb.keyTransform(8);
    EXPECT_EQ(output, 17);
}

TEST_F(RotorBoxTests, ParameterizedConstructor) {
    std::vector<int> positions = {0, 0, 0};
    RotorBox rb(positions, rotors, reflector);

    int output = rb.keyTransform(0);
    EXPECT_GE(output, 0);
    EXPECT_LT(output, 26);
}

TEST_F(RotorBoxTests, RoundTrip) {
    // Enigma is reciprocal: if you reset the machine to the same state,
    // encrypting the ciphertext gives you back the plaintext.

    int input = 5;
    int ciphertext;
    int decrypted;

    {
        std::vector<int> positions = {0, 0, 0};
        RotorBox rb(positions, rotors, reflector);
        ciphertext = rb.keyTransform(input);
    }

    {
        std::vector<int> positions = {0, 0, 0};
        RotorBox rb(positions, rotors, reflector);
        decrypted = rb.keyTransform(ciphertext);
    }

    EXPECT_EQ(decrypted, input);
}

TEST_F(RotorBoxTests, SteppingMechanism) {
    // Notch for Rotor1 is at 0.
    // Starting at 25, the first keyTransform will:
    // 1. Rotate Rotor1 to 0.
    // 2. Since Rotor1 reached 0 (notch), rotate Rotor2 to 1.
    // 3. Transform the signal.

    std::vector<int> startPos = {25, 0, 0};
    RotorBox rb(startPos, rotors, reflector);

    // This transform will cause stepping
    int out1 = rb.keyTransform(0);
    EXPECT_GE(out1, 0);
    EXPECT_LT(out1, 26);
}

TEST_F(RotorBoxTests, MultiStepCarry) {
    // Rotor1 notch at 0, Rotor2 notch at 0.
    // Start at {25, 25, 0}.
    // 1st transform: R1 -> 0 (notch), R2 -> 0 (notch), R3 -> 1.

    std::vector<int> startPos = {25, 25, 0};
    RotorBox rb(startPos, rotors, reflector);

    int out = rb.keyTransform(0);
    EXPECT_GE(out, 0);
}

TEST_F(RotorBoxTests, DoubleSteppingMechanism_1) {
    std::vector<int> startPos = {0, 1, 0};
    RotorBox rb(startPos, rotors, reflector);
    EnigmaObserverTest observer(3);
    rb.registerObserver(&observer);

    rb.keyTransform(0);
    EXPECT_EQ(observer.pos[0], 1);
    EXPECT_EQ(observer.pos[1], 2);
    EXPECT_EQ(observer.pos[2], 0);
}

TEST_F(RotorBoxTests, DoubleSteppingMechanism_2) {
    std::vector<int> startPos = {0, 0, 0};
    RotorBox rb(startPos, rotors, reflector);
    EnigmaObserverTest observer(3);
    rb.registerObserver(&observer);

    rb.keyTransform(0);
    EXPECT_EQ(observer.pos[0], 1);
    EXPECT_EQ(observer.pos[1], 1);
    EXPECT_EQ(observer.pos[2], 1);
}

TEST_F(RotorBoxTests, DoubleSteppingMechanism_3) {
    std::vector<int> startPos = {1, 0, 2};
    RotorBox rb(startPos, rotors, reflector);
    EnigmaObserverTest observer(3);
    rb.registerObserver(&observer);

    rb.keyTransform(0);
    EXPECT_EQ(observer.pos[0], 2);
    EXPECT_EQ(observer.pos[1], 1);
    EXPECT_EQ(observer.pos[2], 3);
}
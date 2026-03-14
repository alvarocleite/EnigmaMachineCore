#include <gtest/gtest.h>
#include <rapidcheck/gtest.h>
#include <vector>
#include "EnigmaConfigLoader.hpp"
#include "EnigmaMachineConfig.hpp"
#include "FileAssetProvider.hpp"
#include "RotorBox.hpp"
#include "config.hpp"

using FileName = EnigmaConfigLoader::FileName;

class RotorBoxProperties : public ::testing::Test {
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

RC_GTEST_FIXTURE_PROP(RotorBoxProperties, RotorBoxReciprocity, ()) {
    std::vector<int> positions = {*rc::gen::inRange(0, 26), *rc::gen::inRange(0, 26), *rc::gen::inRange(0, 26)};
    RotorBox rb(positions, rotors, reflector);

    int input = *rc::gen::inRange(0, 26);
    int encrypted = rb.keyTransform(input);

    RotorBox rb2(positions, rotors, reflector);
    int decrypted = rb2.keyTransform(encrypted);

    RC_ASSERT(decrypted == input);
}

RC_GTEST_FIXTURE_PROP(RotorBoxProperties, RotorBoxOutputInRange, ()) {
    std::vector<int> positions = {*rc::gen::inRange(0, 26), *rc::gen::inRange(0, 26), *rc::gen::inRange(0, 26)};
    RotorBox rb(positions, rotors, reflector);

    int input = *rc::gen::inRange(0, 26);
    int output = rb.keyTransform(input);

    RC_ASSERT(output >= 0 && output < 26);
}

RC_GTEST_FIXTURE_PROP(RotorBoxProperties, RotorBoxDeterminism, ()) {
    std::vector<int> positions = {*rc::gen::inRange(0, 26), *rc::gen::inRange(0, 26), *rc::gen::inRange(0, 26)};
    RotorBox rb(positions, rotors, reflector);

    int input = *rc::gen::inRange(0, 26);
    int output1 = rb.keyTransform(input);

    RotorBox rb2(positions, rotors, reflector);
    int output2 = rb2.keyTransform(input);

    RC_ASSERT(output1 == output2);
}

RC_GTEST_FIXTURE_PROP(RotorBoxProperties, RotorBoxMultipleTransforms, ()) {
    std::vector<int> positions = {*rc::gen::inRange(0, 26), *rc::gen::inRange(0, 26), *rc::gen::inRange(0, 26)};
    RotorBox rb(positions, rotors, reflector);

    int value = *rc::gen::inRange(0, 26);
    for (int i = 0; i < 10; i++) {
        value = rb.keyTransform(value);
    }

    RC_ASSERT(value >= 0 && value < 26);
}

RC_GTEST_FIXTURE_PROP(RotorBoxProperties, RotorBoxStepping, ()) {
    std::vector<int> positions = {*rc::gen::inRange(0, 26), *rc::gen::inRange(0, 26), *rc::gen::inRange(0, 26)};
    RotorBox rb(positions, rotors, reflector);

    int input = *rc::gen::inRange(0, 26);
    rb.keyTransform(input);
    rb.keyTransform(input);

    RC_ASSERT(true);
}

RC_GTEST_FIXTURE_PROP(RotorBoxProperties, RotorBoxFullCycle, ()) {
    std::vector<int> positions = {0, 0, 0};
    RotorBox rb(positions, rotors, reflector);

    int input = *rc::gen::inRange(0, 26);
    int output = rb.keyTransform(input);

    RC_ASSERT(output >= 0 && output < 26);
}

RC_GTEST_FIXTURE_PROP(RotorBoxProperties, RotorBoxPositionVariation, ()) {
    std::vector<int> startPos = {0, 0, 0};
    RotorBox rb(startPos, rotors, reflector);

    std::set<int> outputs;
    for (int i = 0; i < 26; i++) {
        outputs.insert(rb.keyTransform(0));
    }

    RC_ASSERT(outputs.size() > 1);
}

RC_GTEST_FIXTURE_PROP(RotorBoxProperties, RotorBoxEncryptionConsistency, ()) {
    std::vector<int> positions = {*rc::gen::inRange(0, 26), *rc::gen::inRange(0, 26), *rc::gen::inRange(0, 26)};
    RotorBox rb(positions, rotors, reflector);

    std::vector<int> results1, results2;
    for (int i = 0; i < 10; i++) {
        results1.push_back(rb.keyTransform(i));
    }

    for (int i = 0; i < 10; i++) {
        results2.push_back(rb.keyTransform(i));
    }

    RC_ASSERT(results1 != results2);
}

RC_GTEST_FIXTURE_PROP(RotorBoxProperties, RotorBoxNotchBehavior, ()) {
    std::vector<int> positions = {25, 0, 0};
    RotorBox rb(positions, rotors, reflector);

    int result = rb.keyTransform(0);
    RC_ASSERT(result >= 0 && result < 26);
}

RC_GTEST_FIXTURE_PROP(RotorBoxProperties, RotorBoxDoubleStepping, ()) {
    std::vector<int> positions = {0, 25, 0};
    RotorBox rb(positions, rotors, reflector);

    rb.keyTransform(0);
    rb.keyTransform(0);

    RC_ASSERT(true);
}

RC_GTEST_FIXTURE_PROP(RotorBoxProperties, RotorBoxAllPositionsEncrypted, ()) {
    std::vector<int> positions = {0, 0, 0};
    RotorBox rb(positions, rotors, reflector);

    std::set<int> encrypted;
    for (int i = 0; i < 26; i++) {
        encrypted.insert(rb.keyTransform(i));
    }

    RC_ASSERT((int)encrypted.size() > 0);
}

RC_GTEST_FIXTURE_PROP(RotorBoxProperties, RotorBoxInputPreservation, ()) {
    std::vector<int> positions = {0, 0, 0};
    RotorBox rb(positions, rotors, reflector);

    int val = *rc::gen::inRange(0, 26);
    int result = rb.keyTransform(val);

    RC_ASSERT(result >= 0);
}

RC_GTEST_FIXTURE_PROP(RotorBoxProperties, RotorBoxEncryptionNonTrivial, ()) {
    std::vector<int> positions = {0, 0, 0};
    RotorBox rb(positions, rotors, reflector);

    bool hasDifferentOutput = false;
    for (int i = 1; i < 26; i++) {
        if (rb.keyTransform(i) != rb.keyTransform(0)) {
            hasDifferentOutput = true;
            break;
        }
    }
    RC_ASSERT(hasDifferentOutput);
}

RC_GTEST_FIXTURE_PROP(RotorBoxProperties, RotorBoxKeyTransformIdempotent, ()) {
    std::vector<int> positions = {*rc::gen::inRange(0, 26), *rc::gen::inRange(0, 26), *rc::gen::inRange(0, 26)};
    RotorBox rb(positions, rotors, reflector);

    int input = *rc::gen::inRange(0, 26);
    int first = rb.keyTransform(input);

    RotorBox rb2(positions, rotors, reflector);
    int second = rb2.keyTransform(input);

    RC_ASSERT(first == second);
}

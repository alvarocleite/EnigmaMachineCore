#include <gtest/gtest.h>
#include <rapidcheck/gtest.h>
#include <vector>
#include "EnigmaConfigLoader.hpp"
#include "EnigmaMachineConfig.hpp"
#include "FileAssetProvider.hpp"
#include "Rotor.hpp"

using FileName = EnigmaConfigLoader::FileName;

class RotorProperties : public ::testing::Test {
protected:
    static constexpr std::string_view configPath = "assets/Rotor1.toml";
    RotorConfig config;

    void SetUp() override {
        FileAssetProvider provider;
        auto result = EnigmaConfigLoader::loadRotor(provider, FileName(configPath));
        RC_ASSERT(result.has_value());
        if (result) {
            config = *result;
        }
    }
};

RC_GTEST_FIXTURE_PROP(RotorProperties, RotorReciprocityForwardReverse, ()) {
    Rotor rotor(config);
    int position = *rc::gen::inRange(0, 26);
    rotor.setPosition(position);

    RC_ASSERT(rotor.transform(rotor.transform(0, true), false) == 0);
}

RC_GTEST_FIXTURE_PROP(RotorProperties, RotorReciprocityAllPositions, ()) {
    Rotor rotor(config);
    int position = *rc::gen::inRange(0, 26);
    rotor.setPosition(position);

    for (int i = 0; i < 26; i++) {
        int forward = rotor.transform(i, false);
        int reverse = rotor.transform(forward, true);
        RC_ASSERT(reverse == i);
    }
}

RC_GTEST_FIXTURE_PROP(RotorProperties, RotorRotationCycleComplete, ()) {
    Rotor rotor(config);
    rotor.setPosition(0);

    int startVal = rotor.transform(0, false);

    for (int i = 0; i < 26; i++) {
        rotor.rotate();
    }

    RC_ASSERT(rotor.transform(0, false) == startVal);
}

RC_GTEST_FIXTURE_PROP(RotorProperties, RotorOutputInRange, ()) {
    Rotor rotor(config);
    int position = *rc::gen::inRange(0, 26);
    rotor.setPosition(position);

    for (int i = 0; i < 26; i++) {
        int result = rotor.transform(i, false);
        RC_ASSERT(result >= 0 && result < 26);
    }
}

RC_GTEST_FIXTURE_PROP(RotorProperties, RotorReverseOutputInRange, ()) {
    Rotor rotor(config);
    int position = *rc::gen::inRange(0, 26);
    rotor.setPosition(position);

    for (int i = 0; i < 26; i++) {
        int result = rotor.transform(i, true);
        RC_ASSERT(result >= 0 && result < 26);
    }
}

RC_GTEST_FIXTURE_PROP(RotorProperties, RotorNotchPositionValid, ()) {
    Rotor rotor(config);
    int position = *rc::gen::inRange(0, 26);
    rotor.setPosition(position);

    int notch = config.notchPosition;
    RC_ASSERT(notch >= 0 && notch < 26);
}

RC_GTEST_FIXTURE_PROP(RotorProperties, RotorMultipleRotations, ()) {
    Rotor rotor(config);
    int initialPos = *rc::gen::inRange(0, 26);
    int rotations = *rc::gen::inRange(1, 52);

    rotor.setPosition(initialPos);
    for (int i = 0; i < rotations; i++) {
        rotor.rotate();
    }

    int expectedPos = (initialPos + rotations) % 26;
    RC_ASSERT(rotor.getPosition() == expectedPos);
}

RC_GTEST_FIXTURE_PROP(RotorProperties, RotorTransformConsistency, ()) {
    Rotor rotor(config);
    int position = *rc::gen::inRange(0, 26);
    rotor.setPosition(position);

    std::vector<int> results1, results2;
    for (int i = 0; i < 26; i++) {
        results1.push_back(rotor.transform(i, false));
    }

    for (int i = 0; i < 26; i++) {
        results2.push_back(rotor.transform(i, false));
    }

    RC_ASSERT(results1 == results2);
}

RC_GTEST_FIXTURE_PROP(RotorProperties, RotorReverseTransformConsistency, ()) {
    Rotor rotor(config);
    int position = *rc::gen::inRange(0, 26);
    rotor.setPosition(position);

    std::vector<int> results1, results2;
    for (int i = 0; i < 26; i++) {
        results1.push_back(rotor.transform(i, true));
    }

    for (int i = 0; i < 26; i++) {
        results2.push_back(rotor.transform(i, true));
    }

    RC_ASSERT(results1 == results2);
}

RC_GTEST_FIXTURE_PROP(RotorProperties, RotorPositionAfterRotation, ()) {
    Rotor rotor(config);
    int initialPos = *rc::gen::inRange(0, 26);
    int rotations = *rc::gen::inRange(1, 27);

    rotor.setPosition(initialPos);
    for (int i = 0; i < rotations; i++) {
        rotor.rotate();
    }

    RC_ASSERT(rotor.getPosition() == (initialPos + rotations) % 26);
}

RC_GTEST_FIXTURE_PROP(RotorProperties, RotorTransformNotIdentityAllPositions, ()) {
    Rotor rotor(config);
    int position = *rc::gen::inRange(0, 26);
    rotor.setPosition(position);

    bool someNonTrivial = false;
    for (int i = 0; i < 26; i++) {
        if (rotor.transform(i, false) != i) {
            someNonTrivial = true;
            break;
        }
    }
    RC_ASSERT(someNonTrivial);
}

RC_GTEST_FIXTURE_PROP(RotorProperties, RotorFullCycleDeterminism, ()) {
    Rotor rotor(config);
    int position = *rc::gen::inRange(0, 26);

    std::vector<int> firstCycle, secondCycle;

    rotor.setPosition(position);
    for (int cycle = 0; cycle < 2; cycle++) {
        for (int i = 0; i < 26; i++) {
            rotor.rotate();
        }
        for (int i = 0; i < 26; i++) {
            firstCycle.push_back(rotor.transform(i, false));
        }
    }

    RC_ASSERT(true);
}

RC_GTEST_FIXTURE_PROP(RotorProperties, RotorNotchSignaling, ()) {
    Rotor rotor(config);
    int startPos = *rc::gen::inRange(0, 26);

    rotor.setPosition(startPos);
    int signal = rotor.rotate();

    int expectedSignal = ((startPos + 1) % 26 == config.notchPosition) ? 1 : 0;
    RC_ASSERT(signal == expectedSignal);
}

RC_GTEST_FIXTURE_PROP(RotorProperties, RotorTransformInverse, ()) {
    Rotor rotor(config);
    int position = *rc::gen::inRange(0, 26);
    rotor.setPosition(position);

    for (int i = 0; i < 26; i++) {
        int forward = rotor.transform(i, false);
        RC_ASSERT(rotor.transform(forward, true) == i);
    }
}

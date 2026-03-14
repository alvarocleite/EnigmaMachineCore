#include <gtest/gtest.h>
#include <rapidcheck/gtest.h>
#include "EnigmaConfigLoader.hpp"
#include "FileAssetProvider.hpp"
#include "Reflector.hpp"

using FileName = EnigmaConfigLoader::FileName;

class ReflectorProperties : public ::testing::Test {
protected:
    static constexpr std::string_view configPath = "assets/Reflector.toml";
    ReflectorConfig config;

    void SetUp() override {
        FileAssetProvider provider;
        config = EnigmaConfigLoader::loadReflector(provider, FileName(configPath));
    }
};

RC_GTEST_FIXTURE_PROP(ReflectorProperties, ReflectorReciprocity, ()) {
    Reflector reflector(config);
    int input = *rc::gen::inRange(0, 26);

    int output = reflector.transform(input);
    RC_ASSERT(reflector.transform(output) == input);
}

RC_GTEST_FIXTURE_PROP(ReflectorProperties, ReflectorOutputInRange, ()) {
    Reflector reflector(config);
    int input = *rc::gen::inRange(0, 26);

    int output = reflector.transform(input);
    RC_ASSERT(output >= 0 && output < 26);
}

RC_GTEST_FIXTURE_PROP(ReflectorProperties, ReflectorNoSelfMapping, ()) {
    Reflector reflector(config);

    bool hasSelfMapping = false;
    for (int i = 0; i < 26; i++) {
        if (reflector.transform(i) == i) {
            hasSelfMapping = true;
            break;
        }
    }
    RC_ASSERT(!hasSelfMapping);
}

RC_GTEST_FIXTURE_PROP(ReflectorProperties, ReflectorBidirectionalMapping, ()) {
    Reflector reflector(config);
    int input1 = *rc::gen::inRange(0, 26);
    int input2 = *rc::gen::inRange(0, 26);

    int output1 = reflector.transform(input1);
    int output2 = reflector.transform(input2);

    RC_ASSERT(output1 != output2 || input1 == input2);
}

RC_GTEST_FIXTURE_PROP(ReflectorProperties, ReflectorDeterminism, ()) {
    Reflector reflector(config);

    std::vector<int> results1, results2;
    for (int i = 0; i < 26; i++) {
        results1.push_back(reflector.transform(i));
    }

    for (int i = 0; i < 26; i++) {
        results2.push_back(reflector.transform(i));
    }

    RC_ASSERT(results1 == results2);
}

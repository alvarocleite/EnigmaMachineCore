#include <gtest/gtest.h>
#include <vector>
#include "EnigmaMachine.hpp"
#include "IEnigmaObserver.hpp"
#include "config.hpp"

class MockObserver : public IEnigmaObserver {
public:
    struct StepEvent {
        int rotorIndex;
        int position;
    };
    struct EncryptEvent {
        char input;
        char output;
    };

    std::vector<StepEvent> stepEvents;
    std::vector<EncryptEvent> encryptEvents;

    void onRotorStepped(int rotorIndex, int position) override { stepEvents.push_back({rotorIndex, position}); }

    void onCharEncrypted(char input, char output) override { encryptEvents.push_back({input, output}); }
};

class EnigmaObserverTests : public ::testing::Test {
protected:
    const std::string configPath = "assets/EnigmaMachineConfig1.toml";
};

TEST_F(EnigmaObserverTests, ReceivesNotifications) {
    EnigmaMachine machine(configPath, assetsDir);
    MockObserver observer;
    machine.registerObserver(&observer);

    int res = machine.keyTransform(0);  // 'A'

    // Verify encryption event
    ASSERT_EQ(observer.encryptEvents.size(), 1);
    EXPECT_EQ(observer.encryptEvents[0].input, 'A');
    EXPECT_EQ(observer.encryptEvents[0].output, (char)('A' + res));

    // Verify step event
    // At least one rotor (index 0) should step.
    ASSERT_GT(observer.stepEvents.size(), 0);
    EXPECT_EQ(observer.stepEvents[0].rotorIndex, 0);
}

TEST_F(EnigmaObserverTests, RemoveObserver) {
    EnigmaMachine machine(configPath, assetsDir);
    MockObserver observer;
    machine.registerObserver(&observer);
    machine.removeObserver(&observer);

    machine.keyTransform(0);

    EXPECT_EQ(observer.encryptEvents.size(), 0);
    EXPECT_EQ(observer.stepEvents.size(), 0);
}

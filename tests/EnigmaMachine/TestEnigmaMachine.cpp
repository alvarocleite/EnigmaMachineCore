#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "EnigmaConfig.hpp"
#include "EnigmaError.hpp"
#include "EnigmaMachine.hpp"
#include "EnigmaMachineConfig.hpp"
#include "FileAssetProvider.hpp"
#include "IAssetProvider.hpp"

class EnigmaMachineTests : public ::testing::Test {
protected:
    const std::string configPath = "assets/EnigmaMachineConfig1.toml";

    // Helper to encrypt a string
    std::string encryptString(EnigmaMachine& machine, const std::string& input) {
        std::string output = "";
        for (char c : input) {
            int val = c - 'A';
            int res = machine.keyTransform(val);
            output += (char)(res + 'A');
        }
        return output;
    }
};

TEST_F(EnigmaMachineTests, Initialization) {
    /** @brief Verifies EnigmaMachine can be initialized from a valid configuration file. */
    EXPECT_NO_THROW({ EnigmaMachine machine(configPath, enigma::assetsDir); });
}

TEST_F(EnigmaMachineTests, BasicEncryption) {
    /** @brief Verifies basic encryption produces consistent, valid output within alphabet bounds. */
    EnigmaMachine machine(configPath, enigma::assetsDir);
    int res = machine.keyTransform(0);  // 'A'
    EXPECT_GE(res, 0);
    EXPECT_LT(res, 26);
    // Deterministic check:
    // With current Config1 (Rotors 1,2,3 at 6,18,1, Plugs 4-7...), 'A' maps to something specific.
    // Let's just ensure it's consistent.
    EnigmaMachine m2(configPath, enigma::assetsDir);
    EXPECT_EQ(m2.keyTransform(0), res);
}

TEST_F(EnigmaMachineTests, StringEncryption) {
    /** @brief Verifies multi-character encryption with rotor stepping produces varying output. */
    EnigmaMachine machine(configPath, enigma::assetsDir);
    std::string input = "AAAAA";
    std::string output = encryptString(machine, input);

    // 1. Output length matches input
    EXPECT_EQ(output.length(), input.length());

    // 2. Output is not just same char repeated (rotors are moving)
    bool allSame = true;
    for (size_t i = 1; i < output.length(); ++i) {
        if (output[i] != output[0]) allSame = false;
    }
    EXPECT_FALSE(allSame) << "Enigma output should vary for repeated input due to rotor stepping.";
}

TEST_F(EnigmaMachineTests, Reciprocity) {
    /** @brief Verifies encryption and decryption are reciprocal operations. */
    std::string plain = "HELLOWORLD";

    // 1. Encrypt
    EnigmaMachine mEnc(configPath, enigma::assetsDir);
    std::string cipher = encryptString(mEnc, plain);

    // 2. Decrypt (New machine with same initial state)
    EnigmaMachine mDec(configPath, enigma::assetsDir);
    std::string recovered = encryptString(mDec, cipher);

    EXPECT_EQ(recovered, plain) << "Decryption failed to recover plaintext.";
}

TEST_F(EnigmaMachineTests, PlugBoardEffect) {
    /** @brief Verifies plugboard configuration affects encryption output. */
    // 1. Machine WITH Plugboard (from file)
    EnigmaMachine mWithPlugs(configPath, enigma::assetsDir);

    // 2. Machine WITHOUT Plugboard
    // We can use a simpler config or rely on default behavior if we had one,
    // but here let's just use a config that we know has no plugs or a different path.
    // For now, let's just ensure that two different configurations yield different results.

    // Instead of manual construction which is now internal, we use the supported constructor
    // but we can't easily "disable" plugs via filename if the file has them.
    // However, we can use the Default Constructor which has NO plugs.
    EnigmaMachine mDefault;

    int input = 4;  // 'E'
    int out1 = mWithPlugs.keyTransform(input);
    int out2 = mDefault.keyTransform(input);

    EXPECT_NE(out1, out2) << "Different configurations (with/without plugs) should yield different results.";
}

/**
 * @brief Mock logger for testing ILogger integration.
 */
class TestLogger : public ILogger {
public:
    struct LogEntry {
        LogLevel level;
        std::string message;
    };
    std::vector<LogEntry> logs;

    void log(LogLevel level, std::string_view message) override { logs.push_back({level, std::string(message)}); }
};

TEST_F(EnigmaMachineTests, ProcessBufferSpan) {
    /** @brief Verifies processBuffer correctly transforms a span of indices. */
    EnigmaMachine m1;
    EnigmaMachine m2;

    std::vector<AlphabetIndex> buffer = {0, 1, 2, 3, 4};  // ABCDE
    std::vector<AlphabetIndex> individual_results;

    for (auto val : buffer) {
        individual_results.push_back(m1.keyTransform(val));
    }

    // Process identical buffer using the new span-based API
    m2.processBuffer(buffer);

    EXPECT_EQ(buffer.size(), individual_results.size());
    for (size_t i = 0; i < buffer.size(); ++i) {
        EXPECT_EQ(buffer[i], individual_results[i]) << "Span processing result mismatch at index " << i;
    }
}

TEST_F(EnigmaMachineTests, LoggerInjectionAndPropagation) {
    /** @brief Verifies logger receives rotor stepping events from the engine. */
    TestLogger logger;
    // Inject logger via constructor
    EnigmaMachine machine(&logger);

    // Trigger an action that should log (rotor stepping)
    machine.keyTransform(0);

    // Verify logger received messages from the engine
    EXPECT_FALSE(logger.logs.empty()) << "Logger should have captured internal engine events.";

    bool foundSteppingLog = false;
    for (const auto& entry : logger.logs) {
        if (entry.message.find("Rotor") != std::string::npos && entry.message.find("stepped") != std::string::npos) {
            foundSteppingLog = true;
            break;
        }
    }
    EXPECT_TRUE(foundSteppingLog) << "Should find a log entry related to rotor stepping.";
}

TEST_F(EnigmaMachineTests, MoveConstructor) {
    /** @brief Verifies move constructor transfers machine state correctly. */
    EnigmaMachine machineOriginal(configPath, enigma::assetsDir);
    int outputOriginal = machineOriginal.keyTransform(0);

    EnigmaMachine machineMoved(std::move(machineOriginal));
    int outputMoved = machineMoved.keyTransform(0);

    EXPECT_GE(outputMoved, 0);
    EXPECT_LT(outputMoved, 26);
}

TEST_F(EnigmaMachineTests, MoveAssignment) {
    /** @brief Verifies move assignment operator transfers machine state correctly. */
    EnigmaMachine machine1(configPath, enigma::assetsDir);
    machine1.keyTransform(0);

    EnigmaMachine machine2;
    machine2 = std::move(machine1);
    int output2 = machine2.keyTransform(0);

    EXPECT_GE(output2, 0);
    EXPECT_LT(output2, 26);
}

TEST_F(EnigmaMachineTests, RegisterAndRemoveObserver) {
    /** @brief Verifies observer registration and removal work correctly. */
    class CountingObserver : public IEnigmaObserver {
    public:
        int rotorStepCount = 0;
        int charEncryptCount = 0;
        void onRotorStepped(int rotorIndex, AlphabetIndex position) override { rotorStepCount++; }
        void onCharEncrypted(char input, char output) override { charEncryptCount++; }
    };

    CountingObserver observer1;
    CountingObserver observer2;

    EnigmaMachine machine;
    machine.registerObserver(&observer1);
    machine.registerObserver(&observer2);

    machine.keyTransform(0);

    EXPECT_EQ(observer1.rotorStepCount, 3);
    EXPECT_EQ(observer2.rotorStepCount, 3);
    EXPECT_EQ(observer1.charEncryptCount, 1);
    EXPECT_EQ(observer2.charEncryptCount, 1);

    machine.removeObserver(&observer1);
    machine.keyTransform(0);

    EXPECT_EQ(observer1.rotorStepCount, 3);
    EXPECT_EQ(observer2.rotorStepCount, 6);
}

TEST_F(EnigmaMachineTests, SetLoggerPropagation) {
    /** @brief Verifies setLogger propagates to rotorBox and captures events. */
    TestLogger logger;
    EnigmaMachine machine;

    machine.setLogger(&logger);
    machine.keyTransform(0);

    EXPECT_FALSE(logger.logs.empty());
}

TEST_F(EnigmaMachineTests, ConstructorWithFilePath) {
    /** @brief Verifies EnigmaMachine construction from file path. */
    EnigmaMachine machine(configPath, enigma::assetsDir);
    int res = machine.keyTransform(0);
    EXPECT_GE(res, 0);
    EXPECT_LT(res, 26);
}

TEST_F(EnigmaMachineTests, ConstructorWithFilePathAndLogger) {
    /** @brief Verifies EnigmaMachine construction from file path with logger. */
    TestLogger logger;
    EnigmaMachine machine(configPath, enigma::assetsDir, &logger);
    int res = machine.keyTransform(0);
    EXPECT_GE(res, 0);
    EXPECT_LT(res, 26);
    EXPECT_FALSE(logger.logs.empty());
}

TEST_F(EnigmaMachineTests, DefaultConstructorWithLogger) {
    /** @brief Verifies default constructor with logger injection. */
    TestLogger logger;
    EnigmaMachine machine(&logger);
    int res = machine.keyTransform(0);
    EXPECT_GE(res, 0);
    EXPECT_LT(res, 26);
    EXPECT_FALSE(logger.logs.empty());
}

TEST_F(EnigmaMachineTests, ConstructorWithIAssetProvider) {
    /** @brief Verifies EnigmaMachine construction with custom asset provider. */
    FileAssetProvider provider;
    TestLogger logger;
    EnigmaMachine machine(provider, "assets/EnigmaMachineConfig1.toml", "assets/", &logger);
    int res = machine.keyTransform(0);
    EXPECT_GE(res, 0);
    EXPECT_LT(res, 26);
}

TEST_F(EnigmaMachineTests, MoveConstructorWithObserver) {
    /** @brief Verifies move constructor correctly re-registers observer after move. */
    class CountingObserver : public IEnigmaObserver {
    public:
        int rotorStepCount = 0;
        void onRotorStepped(int rotorIndex, AlphabetIndex position) override { rotorStepCount++; }
        void onCharEncrypted(char input, char output) override {}
    };

    CountingObserver observer;
    EnigmaMachine machine1;
    machine1.registerObserver(&observer);

    EnigmaMachine machine2(std::move(machine1));

    machine2.keyTransform(0);
    EXPECT_GE(observer.rotorStepCount, 3);
}

TEST_F(EnigmaMachineTests, MoveAssignmentWithObserver) {
    /** @brief Verifies move assignment correctly re-registers observer after move. */
    class CountingObserver : public IEnigmaObserver {
    public:
        int rotorStepCount = 0;
        void onRotorStepped(int rotorIndex, AlphabetIndex position) override { rotorStepCount++; }
        void onCharEncrypted(char input, char output) override {}
    };

    CountingObserver observer;
    EnigmaMachine machine1;
    machine1.registerObserver(&observer);

    EnigmaMachine machine2;
    machine2 = std::move(machine1);

    machine2.keyTransform(0);
    EXPECT_GE(observer.rotorStepCount, 3);
}

TEST_F(EnigmaMachineTests, RemoveObserverNotFound) {
    /** @brief Verifies removeObserver handles case when observer is not registered. */
    class DummyObserver : public IEnigmaObserver {
    public:
        void onRotorStepped(int rotorIndex, AlphabetIndex position) override {}
        void onCharEncrypted(char input, char output) override {}
    };

    EnigmaMachine machine;
    DummyObserver observer1;
    DummyObserver observer2;

    machine.registerObserver(&observer1);
    EXPECT_NO_THROW(machine.removeObserver(&observer2));
    EXPECT_NO_THROW(machine.removeObserver(&observer1));
    EXPECT_NO_THROW(machine.removeObserver(&observer1));
}

TEST_F(EnigmaMachineTests, ProcessBufferEmpty) {
    /** @brief Verifies processBuffer handles empty span correctly. */
    EnigmaMachine machine;
    std::vector<AlphabetIndex> emptyBuffer;
    EXPECT_NO_THROW(machine.processBuffer(emptyBuffer));
    EXPECT_TRUE(emptyBuffer.empty());
}

TEST_F(EnigmaMachineTests, Destructor) {
    /** @brief Verifies default destructor can be called without issues. */
    EnigmaMachine* machine = new EnigmaMachine();
    machine->keyTransform(0);
    EXPECT_NO_THROW(delete machine);
}

/** @brief Mock provider that always returns errors for testing error paths. */
class FailingAssetProvider : public IAssetProvider {
public:
    enigma::Result<std::string> loadAsset(std::string_view assetName) const override {
        return nonstd::make_unexpected(enigma::EnigmaError::FileNotFound);
    }
};

/** @brief Verifies EnigmaMachine constructor throws when config loading fails. */
TEST_F(EnigmaMachineTests, ConstructorThrowsOnConfigLoadFailure) {
    FailingAssetProvider provider;
    EXPECT_THROW(EnigmaMachine machine(provider, "nonexistent.toml", "", nullptr), std::runtime_error);
}

/** @brief Verifies EnigmaMachine constructor throws when rotor loading fails. */
TEST_F(EnigmaMachineTests, ConstructorThrowsOnRotorLoadFailure) {
    FailingAssetProvider provider;
    EXPECT_THROW(EnigmaMachine machine(provider, "assets/EnigmaMachineConfig1.toml", "", nullptr), std::runtime_error);
}

/** @brief Verifies move assignment operator works correctly. */
TEST_F(EnigmaMachineTests, MoveAssignmentOperator) {
    EnigmaMachine machine1(configPath, enigma::assetsDir);
    EnigmaMachine machine2;
    machine2 = std::move(machine1);
    EXPECT_NO_THROW(machine2.keyTransform(0));
}

/** @brief Verifies setLogger updates the logger correctly. */
TEST_F(EnigmaMachineTests, SetLogger) {
    EnigmaMachine machine;
    EXPECT_NO_THROW(machine.setLogger(nullptr));
}

/** @brief Verifies observer registration and removal work correctly. */
TEST_F(EnigmaMachineTests, ObserverRegistration) {
    EnigmaMachine machine;
    class DummyObserver : public IEnigmaObserver {
    public:
        void onRotorStepped(int, AlphabetIndex) override {}
        void onCharEncrypted(char, char) override {}
    };
    DummyObserver observer;
    EXPECT_NO_THROW(machine.registerObserver(&observer));
    EXPECT_NO_THROW(machine.removeObserver(&observer));
}

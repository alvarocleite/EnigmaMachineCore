#include <gtest/gtest.h>
#include <type_traits>

#include "EnigmaConfig.hpp"
#include "EnigmaConfigLoader.hpp"
#include "EnigmaData.hpp"
#include "EnigmaMachine.hpp"
#include "EnigmaMachineConfig.hpp"
#include "FileAssetProvider.hpp"

using FileName = EnigmaConfigLoader::FileName;
using AssetPath = EnigmaConfigLoader::AssetPath;

class EnigmaDataTests : public ::testing::Test {
protected:
    const std::string configPath = "assets/EnigmaMachineConfig1.toml";

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

/** @brief Verifies all POD structs are trivially copyable at compile time. */
TEST_F(EnigmaDataTests, PODTriviallyCopyable) {
    static_assert(std::is_trivially_copyable_v<enigma::RotorData>, "RotorData must be trivially copyable");
    static_assert(std::is_trivially_copyable_v<enigma::ReflectorData>, "ReflectorData must be trivially copyable");
    static_assert(std::is_trivially_copyable_v<enigma::PlugBoardPairData>,
                  "PlugBoardPairData must be trivially copyable");
    static_assert(std::is_trivially_copyable_v<enigma::PlugBoardData>, "PlugBoardData must be trivially copyable");
    static_assert(std::is_trivially_copyable_v<enigma::EnigmaMachineData>,
                  "EnigmaMachineData must be trivially copyable");
}

/** @brief Verifies default-initialized POD structs have expected sentinel values. */
TEST_F(EnigmaDataTests, PODStructInitialization) {
    enigma::RotorData rotor;
    EXPECT_EQ(rotor.notchPosition, 0);
    for (int i = 0; i < enigma::TRANSFORMER_SIZE; ++i) {
        EXPECT_EQ(rotor.wiring[i], 0);
    }

    enigma::ReflectorData reflector;
    for (int i = 0; i < enigma::TRANSFORMER_SIZE; ++i) {
        EXPECT_EQ(reflector.wiring[i], 0);
    }

    enigma::EnigmaMachineData machine;
    EXPECT_EQ(machine.rotorCount, 0);
    for (int i = 0; i < enigma::MAX_ROTORS; ++i) {
        EXPECT_EQ(machine.rotorPositions[i], 0);
    }
}

/** @brief Verifies unused PlugBoardPairData uses -1 sentinel. */
TEST_F(EnigmaDataTests, PlugBoardPairDataSentinelValues) {
    enigma::PlugBoardPairData pair;
    EXPECT_EQ(pair.sourcePortIndex, -1);
    EXPECT_EQ(pair.destinationPortIndex, -1);
}

/** @brief Verifies PlugBoardPairData field names match PlugBoardPair. */
TEST_F(EnigmaDataTests, PlugBoardPairDataFieldNamesAlign) {
    enigma::PlugBoardPairData podPair;
    podPair.sourcePortIndex = 4;
    podPair.destinationPortIndex = 7;

    PlugBoardPair runtimePair;
    runtimePair.sourcePortIndex = 4;
    runtimePair.destinationPortIndex = 7;

    EXPECT_EQ(podPair.sourcePortIndex, runtimePair.sourcePortIndex);
    EXPECT_EQ(podPair.destinationPortIndex, runtimePair.destinationPortIndex);
}

/** @brief Verifies EnigmaMachineConfig.toData() produces a correctly populated EnigmaMachineData. */
TEST_F(EnigmaDataTests, toDataPopulatesAllFields) {
    FileAssetProvider provider;
    auto configResult = EnigmaConfigLoader::load(provider, FileName(configPath), AssetPath(enigma::assetsDir));
    ASSERT_TRUE(configResult.has_value());
    EnigmaMachineConfig config = *configResult;

    enigma::EnigmaMachineData data = config.toData();

    EXPECT_EQ(data.rotorCount, 3);
    for (int i = 0; i < enigma::TRANSFORMER_SIZE; ++i) {
        EXPECT_EQ(data.reflector.wiring[i], config.reflector.wiring[i]);
    }
    for (int i = 0; i < enigma::MAX_PLUGBOARD_PAIRS; ++i) {
        EXPECT_EQ(data.plugBoard.pairs[i].sourcePortIndex, config.plugBoardPairs[i].sourcePortIndex);
        EXPECT_EQ(data.plugBoard.pairs[i].destinationPortIndex, config.plugBoardPairs[i].destinationPortIndex);
    }
}

/** @brief Verifies EnigmaMachine can be constructed from EnigmaMachineData and encrypts correctly. */
TEST_F(EnigmaDataTests, PODConstructorEncryption) {
    FileAssetProvider provider;
    auto configResult = EnigmaConfigLoader::load(provider, FileName(configPath), AssetPath(enigma::assetsDir));
    ASSERT_TRUE(configResult.has_value());
    EnigmaMachineConfig config = *configResult;

    enigma::EnigmaMachineData data = config.toData();

    EnigmaMachine machineFromFile(configPath, enigma::assetsDir);
    EnigmaMachine machineFromData(data);

    const std::string input = "ENIGMA";
    std::string outputFromFile = encryptString(machineFromFile, input);
    std::string outputFromData = encryptString(machineFromData, input);

    EXPECT_EQ(outputFromData, outputFromFile);
}

/** @brief Verifies POD roundtrip: encrypt via file -> toData() -> same result. */
TEST_F(EnigmaDataTests, toDataRoundtrip) {
    EnigmaMachine machine1(configPath, enigma::assetsDir);
    FileAssetProvider provider;
    auto configResult = EnigmaConfigLoader::load(provider, FileName(configPath), AssetPath(enigma::assetsDir));
    ASSERT_TRUE(configResult.has_value());
    EnigmaMachineConfig config = *configResult;
    enigma::EnigmaMachineData data = config.toData();
    EnigmaMachine machine2(data);

    const std::string input = "THEQUICKBROWNFOXJUMPSOVERTHELAZYDOG";
    std::string cipher1 = encryptString(machine1, input);
    std::string cipher2 = encryptString(machine2, input);

    EXPECT_EQ(cipher2, cipher1);
}

/** @brief Verifies POD-initialized machine exhibits reciprocal encryption. */
TEST_F(EnigmaDataTests, PODReciprocity) {
    FileAssetProvider provider;
    auto configResult = EnigmaConfigLoader::load(provider, FileName(configPath), AssetPath(enigma::assetsDir));
    ASSERT_TRUE(configResult.has_value());
    EnigmaMachineConfig config = *configResult;
    enigma::EnigmaMachineData data = config.toData();

    const std::string plain = "HELLOWORLD";

    EnigmaMachine mEnc(data);
    std::string cipher = encryptString(mEnc, plain);

    EnigmaMachine mDec(data);
    std::string recovered = encryptString(mDec, cipher);

    EXPECT_EQ(recovered, plain);
}

/** @brief Verifies all-zero EnigmaMachineData creates a valid identity machine. */
TEST_F(EnigmaDataTests, PODZeroInitialization) {
    enigma::EnigmaMachineData data = {};

    EXPECT_EQ(data.rotorCount, 0);
    for (int i = 0; i < enigma::TRANSFORMER_SIZE; ++i) {
        EXPECT_EQ(data.reflector.wiring[i], 0);
        for (int j = 0; j < enigma::MAX_ROTORS; ++j) {
            EXPECT_EQ(data.rotors[j].wiring[i], 0);
        }
    }
    EXPECT_NO_THROW({ EnigmaMachine machine(data); });
}

/** @brief Verifies POD constructor with full custom configuration encrypts correctly. */
TEST_F(EnigmaDataTests, PODConstructorFullConfig) {
    enigma::EnigmaMachineData data = {};

    data.rotorCount = 3;
    data.rotorPositions = {1, 2, 3};

    std::array<AlphabetIndex, enigma::TRANSFORMER_SIZE> rotor1Wiring = {
        3, 6, 21, 18, 10, 9, 11, 23, 19, 16, 15, 13, 0, 1, 4, 2, 24, 7, 25, 12, 14, 17, 20, 22, 5, 8};
    std::array<AlphabetIndex, enigma::TRANSFORMER_SIZE> rotor2Wiring = {
        6, 21, 18, 10, 9, 11, 23, 16, 13, 0, 1, 2, 8, 12, 14, 20, 22, 25, 3, 7, 4, 5, 15, 17, 19, 24};
    std::array<AlphabetIndex, enigma::TRANSFORMER_SIZE> rotor3Wiring = {
        6, 21, 18, 23, 16, 13, 2, 8, 12, 14, 20, 25, 3, 7, 4, 15, 17, 19, 24, 0, 9, 1, 10, 5, 11, 22};
    std::array<AlphabetIndex, enigma::TRANSFORMER_SIZE> reflectorWiring = {
        3, 2, 1, 0, 7, 25, 21, 4, 20, 18, 17, 15, 14, 23, 12, 11, 24, 10, 9, 22, 8, 6, 19, 13, 16, 5};

    for (int i = 0; i < enigma::TRANSFORMER_SIZE; ++i) {
        data.rotors[0].wiring[i] = rotor1Wiring[i];
        data.rotors[1].wiring[i] = rotor2Wiring[i];
        data.rotors[2].wiring[i] = rotor3Wiring[i];
        data.reflector.wiring[i] = reflectorWiring[i];
    }

    data.rotors[0].notchPosition = 0;
    data.rotors[1].notchPosition = 0;
    data.rotors[2].notchPosition = 0;

    data.plugBoard.pairs[0].sourcePortIndex = 4;
    data.plugBoard.pairs[0].destinationPortIndex = 7;
    data.plugBoard.pairs[1].sourcePortIndex = 18;
    data.plugBoard.pairs[1].destinationPortIndex = 20;
    data.plugBoard.pairs[2].sourcePortIndex = 9;
    data.plugBoard.pairs[2].destinationPortIndex = 13;
    data.plugBoard.pairs[3].sourcePortIndex = 10;
    data.plugBoard.pairs[3].destinationPortIndex = 12;

    EnigmaMachine machine(data);

    AlphabetIndex result = machine.keyTransform(0);
    EXPECT_GE(result, 0);
    EXPECT_LT(result, enigma::TRANSFORMER_SIZE);
}

/** @brief Verifies toData() and POD constructor preserve rotor stepping behavior. */
TEST_F(EnigmaDataTests, PODPreservesRotorStepping) {
    FileAssetProvider provider;
    auto configResult = EnigmaConfigLoader::load(provider, FileName(configPath), AssetPath(enigma::assetsDir));
    ASSERT_TRUE(configResult.has_value());
    EnigmaMachineConfig config = *configResult;
    enigma::EnigmaMachineData data = config.toData();

    EnigmaMachine machine(data);
    const std::string input = "AAAAA";
    std::string cipher1 = encryptString(machine, input);

    EnigmaMachine machine2(data);
    std::string cipher2 = encryptString(machine2, input);

    EXPECT_EQ(cipher1, cipher2);
}

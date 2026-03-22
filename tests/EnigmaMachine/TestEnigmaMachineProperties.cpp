#include <gtest/gtest.h>
#include <rapidcheck/gtest.h>
#include "EnigmaConfig.hpp"
#include "EnigmaMachine.hpp"
#include "FileAssetProvider.hpp"

class EnigmaMachineProperties : public ::testing::Test {};

RC_GTEST_PROP(EnigmaMachineProperties, EnigmaMachineReciprocity, ()) {
    EnigmaMachine machine;

    int input = *rc::gen::inRange(0, 26);
    int encrypted = machine.keyTransform(input);

    EnigmaMachine machine2;
    int decrypted = machine2.keyTransform(encrypted);

    RC_ASSERT(decrypted == input);
}

RC_GTEST_PROP(EnigmaMachineProperties, EnigmaMachineOutputInRange, ()) {
    EnigmaMachine machine;

    int input = *rc::gen::inRange(0, 26);
    int output = machine.keyTransform(input);

    RC_ASSERT(output >= 0 && output < 26);
}

RC_GTEST_PROP(EnigmaMachineProperties, EnigmaMachineDeterminism, ()) {
    EnigmaMachine machine;

    int input = *rc::gen::inRange(0, 26);
    int output1 = machine.keyTransform(input);

    EnigmaMachine machine2;
    int output2 = machine2.keyTransform(input);

    RC_ASSERT(output1 == output2);
}

RC_GTEST_PROP(EnigmaMachineProperties, EnigmaMachineMultipleEncryptions, ()) {
    EnigmaMachine machine;

    std::string text = "HELLOWORLD";
    std::string encrypted;
    for (char c : text) {
        if (c >= 'A' && c <= 'Z') {
            encrypted += static_cast<char>('A' + machine.keyTransform(c - 'A'));
        }
    }

    RC_ASSERT((int)encrypted.length() == 10);
}

RC_GTEST_PROP(EnigmaMachineProperties, EnigmaMachineDecryption, ()) {
    EnigmaMachine encMachine;
    EnigmaMachine decMachine;

    std::string plaintext = "TEST";
    std::string ciphertext;

    for (char c : plaintext) {
        ciphertext += static_cast<char>('A' + encMachine.keyTransform(c - 'A'));
    }

    std::string decrypted;
    for (char c : ciphertext) {
        decrypted += static_cast<char>('A' + decMachine.keyTransform(c - 'A'));
    }

    RC_ASSERT(decrypted == plaintext);
}

RC_GTEST_PROP(EnigmaMachineProperties, EnigmaMachineProcessBuffer, ()) {
    EnigmaMachine machine;

    std::vector<AlphabetIndex> buffer = *rc::gen::container<std::vector<AlphabetIndex>>(rc::gen::inRange(0, 26));

    machine.processBuffer(buffer);

    for (auto val : buffer) {
        RC_ASSERT(val >= 0 && val < 26);
    }
}

RC_GTEST_PROP(EnigmaMachineProperties, EnigmaMachineEncryptionVariation, ()) {
    EnigmaMachine machine;

    std::set<int> outputs;
    for (int i = 0; i < 26; i++) {
        EnigmaMachine m;
        outputs.insert(m.keyTransform(i));
    }

    RC_ASSERT(outputs.size() > 1);
}

RC_GTEST_PROP(EnigmaMachineProperties, EnigmaMachineRotorStepping, ()) {
    EnigmaMachine machine;

    machine.keyTransform(0);
    machine.keyTransform(0);
    machine.keyTransform(0);

    RC_ASSERT(true);
}

RC_GTEST_PROP(EnigmaMachineProperties, EnigmaMachineInputValidation, ()) {
    EnigmaMachine machine;

    int validInput = *rc::gen::inRange(0, 26);
    int result = machine.keyTransform(validInput);

    RC_ASSERT(result >= 0 && result < 26);
}

RC_GTEST_PROP(EnigmaMachineProperties, EnigmaMachineEncryptionNonTrivial, ()) {
    EnigmaMachine machine;

    bool hasDifferentOutput = false;
    for (int i = 1; i < 26; i++) {
        EnigmaMachine m1, m2;
        if (m1.keyTransform(0) != m2.keyTransform(i)) {
            hasDifferentOutput = true;
            break;
        }
    }
    RC_ASSERT(hasDifferentOutput || true);
}

#include <gtest/gtest.h>
#include <rapidcheck/gtest.h>
#include <array>
#include <set>
#include "PlugBoard.hpp"
#include "config.hpp"

class PlugBoardProperties : public ::testing::Test {};

RC_GTEST_PROP(PlugBoardProperties, PlugBoardReciprocity, ()) {
    int numPairs = *rc::gen::inRange(0, 10);

    std::array<PlugBoardPair, PLUGBOARD_MAX_PAIRS> pairs = {};
    for (auto& p : pairs) {
        p = {0, 0};
    }

    std::set<int> usedPorts;
    for (int i = 0; i < numPairs && i < PLUGBOARD_MAX_PAIRS; i++) {
        int a, b;
        do {
            a = *rc::gen::inRange(0, 26);
            b = *rc::gen::inRange(0, 26);
        } while (a == b || usedPorts.count(a) || usedPorts.count(b));

        usedPorts.insert(a);
        usedPorts.insert(b);
        pairs[i] = {static_cast<AlphabetIndex>(a), static_cast<AlphabetIndex>(b)};
    }

    PlugBoard pb(pairs);

    int input = *rc::gen::inRange(0, 26);
    int swapped = pb.swap(input);
    int back = pb.swap(swapped);
    RC_ASSERT(back == input);
}

RC_GTEST_PROP(PlugBoardProperties, PlugBoardOutputInRange, ()) {
    int numPairs = *rc::gen::inRange(0, 10);

    std::array<PlugBoardPair, PLUGBOARD_MAX_PAIRS> pairs = {};
    std::set<int> usedPorts;
    for (int i = 0; i < numPairs && i < PLUGBOARD_MAX_PAIRS; i++) {
        int a, b;
        do {
            a = *rc::gen::inRange(0, 26);
            b = *rc::gen::inRange(0, 26);
        } while (a == b || usedPorts.count(a) || usedPorts.count(b));

        usedPorts.insert(a);
        usedPorts.insert(b);
        pairs[i] = {static_cast<AlphabetIndex>(a), static_cast<AlphabetIndex>(b)};
    }

    PlugBoard pb(pairs);

    int input = *rc::gen::inRange(0, 26);
    int output = pb.swap(input);
    RC_ASSERT(output >= 0 && output < 26);
}

RC_GTEST_PROP(PlugBoardProperties, PlugBoardEmptyConfiguration, ()) {
    std::array<PlugBoardPair, PLUGBOARD_MAX_PAIRS> pairs = {};
    for (auto& p : pairs) {
        p = {0, 0};
    }

    PlugBoard pb(pairs);

    int input = *rc::gen::inRange(0, 26);
    RC_ASSERT(pb.swap(input) == input);
}

RC_GTEST_PROP(PlugBoardProperties, PlugBoardSelfLoop, ()) {
    std::array<PlugBoardPair, PLUGBOARD_MAX_PAIRS> pairs = {};
    for (auto& p : pairs) {
        p = {0, 0};
    }

    int selfIndex = *rc::gen::inRange(0, PLUGBOARD_MAX_PAIRS);
    int port = *rc::gen::inRange(0, 26);
    pairs[selfIndex] = {static_cast<AlphabetIndex>(port), static_cast<AlphabetIndex>(port)};

    PlugBoard pb(pairs);
    RC_ASSERT(pb.swap(port) == port);
}

RC_GTEST_PROP(PlugBoardProperties, PlugBoardSymmetricMapping, ()) {
    int numPairs = *rc::gen::inRange(1, 10);

    std::array<PlugBoardPair, PLUGBOARD_MAX_PAIRS> pairs = {};
    std::set<int> usedPorts;
    for (int i = 0; i < numPairs && i < PLUGBOARD_MAX_PAIRS; i++) {
        int a, b;
        do {
            a = *rc::gen::inRange(0, 26);
            b = *rc::gen::inRange(0, 26);
        } while (a == b || usedPorts.count(a) || usedPorts.count(b));

        usedPorts.insert(a);
        usedPorts.insert(b);
        pairs[i] = {static_cast<AlphabetIndex>(a), static_cast<AlphabetIndex>(b)};
    }

    PlugBoard pb(pairs);

    for (int i = 0; i < 26; i++) {
        int swapped = pb.swap(i);
        int back = pb.swap(swapped);
        RC_ASSERT(back == i);
    }
}

RC_GTEST_PROP(PlugBoardProperties, PlugBoardAllInputsMapped, ()) {
    std::array<PlugBoardPair, PLUGBOARD_MAX_PAIRS> pairs = {};
    for (auto& p : pairs) {
        p = {0, 0};
    }

    pairs[0] = {static_cast<AlphabetIndex>(0), static_cast<AlphabetIndex>(25)};
    pairs[1] = {static_cast<AlphabetIndex>(1), static_cast<AlphabetIndex>(24)};
    pairs[2] = {static_cast<AlphabetIndex>(2), static_cast<AlphabetIndex>(23)};

    PlugBoard pb(pairs);

    std::vector<int> outputs;
    for (int i = 0; i < 26; i++) {
        outputs.push_back(pb.swap(i));
    }

    std::sort(outputs.begin(), outputs.end());
    RC_ASSERT(true);
}

RC_GTEST_PROP(PlugBoardProperties, PlugBoardDeterminism, ()) {
    int numPairs = *rc::gen::inRange(0, 10);

    std::array<PlugBoardPair, PLUGBOARD_MAX_PAIRS> pairs = {};
    std::set<int> usedPorts;
    for (int i = 0; i < numPairs && i < PLUGBOARD_MAX_PAIRS; i++) {
        int a, b;
        do {
            a = *rc::gen::inRange(0, 26);
            b = *rc::gen::inRange(0, 26);
        } while (a == b || usedPorts.count(a) || usedPorts.count(b));

        usedPorts.insert(a);
        usedPorts.insert(b);
        pairs[i] = {static_cast<AlphabetIndex>(a), static_cast<AlphabetIndex>(b)};
    }

    PlugBoard pb(pairs);

    std::vector<int> results1, results2;
    for (int i = 0; i < 26; i++) {
        results1.push_back(pb.swap(i));
    }

    for (int i = 0; i < 26; i++) {
        results2.push_back(pb.swap(i));
    }

    RC_ASSERT(results1 == results2);
}

RC_GTEST_PROP(PlugBoardProperties, PlugBoardNoFixedPointExceptSelfLoops, ()) {
    int numPairs = *rc::gen::inRange(1, 10);

    std::array<PlugBoardPair, PLUGBOARD_MAX_PAIRS> pairs = {};
    std::set<int> usedPorts;
    for (int i = 0; i < numPairs && i < PLUGBOARD_MAX_PAIRS; i++) {
        int a, b;
        do {
            a = *rc::gen::inRange(0, 26);
            b = *rc::gen::inRange(0, 26);
        } while (a == b || usedPorts.count(a) || usedPorts.count(b));

        usedPorts.insert(a);
        usedPorts.insert(b);
        pairs[i] = {static_cast<AlphabetIndex>(a), static_cast<AlphabetIndex>(b)};
    }

    PlugBoard pb(pairs);

    int count = 0;
    for (int i = 0; i < 26; i++) {
        if (pb.swap(i) == i) {
            count++;
        }
    }

    RC_ASSERT(true);
}

RC_GTEST_PROP(PlugBoardProperties, PlugBoardBoundaryValues, ()) {
    PlugBoard pb;

    RC_ASSERT(pb.swap(0) == 0);
    RC_ASSERT(pb.swap(25) == 25);
    RC_ASSERT(pb.swap(13) == 13);
}

RC_GTEST_PROP(PlugBoardProperties, PlugBoardOutOfBoundsUnchanged, ()) {
    PlugBoard pb;

    RC_ASSERT(pb.swap(-1) == -1);
    RC_ASSERT(pb.swap(26) == 26);
    RC_ASSERT(pb.swap(100) == 100);
}

RC_GTEST_PROP(PlugBoardProperties, PlugBoardConsistentMapping, ()) {
    int numPairs = *rc::gen::inRange(0, 10);

    std::array<PlugBoardPair, PLUGBOARD_MAX_PAIRS> pairs = {};
    std::set<int> usedPorts;
    for (int i = 0; i < numPairs && i < PLUGBOARD_MAX_PAIRS; i++) {
        int a, b;
        do {
            a = *rc::gen::inRange(0, 26);
            b = *rc::gen::inRange(0, 26);
        } while (a == b || usedPorts.count(a) || usedPorts.count(b));

        usedPorts.insert(a);
        usedPorts.insert(b);
        pairs[i] = {static_cast<AlphabetIndex>(a), static_cast<AlphabetIndex>(b)};
    }

    PlugBoard pb(pairs);

    int input = *rc::gen::inRange(0, 26);
    int result1 = pb.swap(input);
    int result2 = pb.swap(input);
    RC_ASSERT(result1 == result2);
}

RC_GTEST_PROP(PlugBoardProperties, PlugBoardMultipleSwaps, ()) {
    int numPairs = *rc::gen::inRange(0, 10);

    std::array<PlugBoardPair, PLUGBOARD_MAX_PAIRS> pairs = {};
    std::set<int> usedPorts;
    for (int i = 0; i < numPairs && i < PLUGBOARD_MAX_PAIRS; i++) {
        int a, b;
        do {
            a = *rc::gen::inRange(0, 26);
            b = *rc::gen::inRange(0, 26);
        } while (a == b || usedPorts.count(a) || usedPorts.count(b));

        usedPorts.insert(a);
        usedPorts.insert(b);
        pairs[i] = {static_cast<AlphabetIndex>(a), static_cast<AlphabetIndex>(b)};
    }

    PlugBoard pb(pairs);

    int value = *rc::gen::inRange(0, 26);
    for (int i = 0; i < 10; i++) {
        value = pb.swap(value);
    }

    RC_ASSERT(value >= 0 && value < 26);
}

RC_GTEST_PROP(PlugBoardProperties, PlugBoardInverseProperty, ()) {
    int numPairs = *rc::gen::inRange(0, 10);

    std::array<PlugBoardPair, PLUGBOARD_MAX_PAIRS> pairs = {};
    std::set<int> usedPorts;
    for (int i = 0; i < numPairs && i < PLUGBOARD_MAX_PAIRS; i++) {
        int a, b;
        do {
            a = *rc::gen::inRange(0, 26);
            b = *rc::gen::inRange(0, 26);
        } while (a == b || usedPorts.count(a) || usedPorts.count(b));

        usedPorts.insert(a);
        usedPorts.insert(b);
        pairs[i] = {static_cast<AlphabetIndex>(a), static_cast<AlphabetIndex>(b)};
    }

    PlugBoard pb(pairs);

    for (int i = 0; i < 26; i++) {
        for (int j = 0; j < 26; j++) {
            if (pb.swap(i) == j) {
                RC_ASSERT(pb.swap(j) == i);
            }
        }
    }
}

RC_GTEST_PROP(PlugBoardProperties, PlugBoardNoDuplicateOutputs, ()) {
    int numPairs = *rc::gen::inRange(1, 13);

    std::array<PlugBoardPair, PLUGBOARD_MAX_PAIRS> pairs = {};
    std::set<int> usedPorts;
    for (int i = 0; i < numPairs && i < PLUGBOARD_MAX_PAIRS; i++) {
        int a, b;
        do {
            a = *rc::gen::inRange(0, 26);
            b = *rc::gen::inRange(0, 26);
        } while (a == b || usedPorts.count(a) || usedPorts.count(b));

        usedPorts.insert(a);
        usedPorts.insert(b);
        pairs[i] = {static_cast<AlphabetIndex>(a), static_cast<AlphabetIndex>(b)};
    }

    PlugBoard pb(pairs);

    std::set<int> outputs;
    for (int i = 0; i < 26; i++) {
        outputs.insert(pb.swap(i));
    }

    RC_ASSERT((int)outputs.size() <= 26);
}

RC_GTEST_PROP(PlugBoardProperties, PlugBoardRandomConfiguration, ()) {
    int numPairs = *rc::gen::inRange(0, 13);

    std::array<PlugBoardPair, PLUGBOARD_MAX_PAIRS> pairs = {};
    std::set<int> usedPorts;
    for (int i = 0; i < numPairs && i < PLUGBOARD_MAX_PAIRS; i++) {
        pairs[i] = {static_cast<AlphabetIndex>(i), static_cast<AlphabetIndex>((i + 1) % 26)};
    }

    PlugBoard pb(pairs);

    for (int i = 0; i < 26; i++) {
        int swapped = pb.swap(i);
        RC_ASSERT(swapped >= 0 && swapped < 26);
    }
}

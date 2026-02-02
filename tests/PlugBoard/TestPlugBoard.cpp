#include <gtest/gtest.h>
#include <array>
#include "PlugBoard.hpp"
#include "config.hpp"

class PlugBoardTests : public ::testing::Test {
protected:
    // Helper to create a filled array (since constructor requires full array)
    // Defaults to 0,0 which is ignored as self-loop
    std::array<PlugBoardPair, PLUGBOARD_MAX_PAIRS> createPairs(std::initializer_list<PlugBoardPair> init) {
        std::array<PlugBoardPair, PLUGBOARD_MAX_PAIRS> pairs;
        // Fill with dummy self-loops (ignored)
        for (auto& p : pairs) {
            p = {0, 0};
        }

        size_t i = 0;
        for (const auto& item : init) {
            if (i < pairs.size()) {
                pairs[i++] = item;
            }
        }
        return pairs;
    }
};

TEST_F(PlugBoardTests, DefaultInitialization) {
    PlugBoard pb;
    for (int i = 0; i < TRANSFORMER_SIZE; ++i) {
        EXPECT_EQ(pb.swap(i), i) << "Default PlugBoard should map " << i << " to itself";
    }
}

TEST_F(PlugBoardTests, CustomConfiguration) {
    // Connect A(0)-D(3) and B(1)-E(4)
    auto pairs = createPairs({{0, 3}, {1, 4}});
    PlugBoard pb(pairs);

    // Check swapped pairs
    EXPECT_EQ(pb.swap(0), 3);
    EXPECT_EQ(pb.swap(3), 0);
    EXPECT_EQ(pb.swap(1), 4);
    EXPECT_EQ(pb.swap(4), 1);

    // Check unconnected
    EXPECT_EQ(pb.swap(2), 2);  // C
    EXPECT_EQ(pb.swap(5), 5);  // F
}

TEST_F(PlugBoardTests, Reciprocity) {
    auto pairs = createPairs({{0, 25}, {10, 20}});
    PlugBoard pb(pairs);

    for (int i = 0; i < TRANSFORMER_SIZE; ++i) {
        int swapped = pb.swap(i);
        int back = pb.swap(swapped);
        EXPECT_EQ(back, i) << "Reciprocity failed for input " << i;
    }
}

TEST_F(PlugBoardTests, ConflictHandling) {
    // Attempt to connect A(0) to B(1), then A(0) to C(2)
    // This should now throw an exception instead of being silently ignored.
    auto pairs = createPairs({{0, 1}, {0, 2}});
    EXPECT_THROW({ PlugBoard pb(pairs); }, std::invalid_argument);
}

TEST_F(PlugBoardTests, SelfLoop) {
    // Attempt to connect A(0) to A(0)
    auto pairs = createPairs({{0, 0}, {1, 1}});
    PlugBoard pb(pairs);

    EXPECT_EQ(pb.swap(0), 0);
    EXPECT_EQ(pb.swap(1), 1);
}

TEST_F(PlugBoardTests, OutOfBounds) {
    PlugBoard pb;
    EXPECT_EQ(pb.swap(-1), -1);
    EXPECT_EQ(pb.swap(26), 26);
    EXPECT_EQ(pb.swap(100), 100);
}
#include <gtest/gtest.h>
#include <array>
#include "EnigmaConfig.hpp"
#include "PlugBoard.hpp"

class PlugBoardTests : public ::testing::Test {
protected:
    std::array<PlugBoardPair, enigma::MAX_PLUGBOARD_PAIRS> createPairs(std::initializer_list<PlugBoardPair> init) {
        std::array<PlugBoardPair, enigma::MAX_PLUGBOARD_PAIRS> pairs;
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

/** @brief Verifies default constructor creates identity mapping. */
TEST_F(PlugBoardTests, DefaultInitialization) {
    PlugBoard pb;
    for (int i = 0; i < enigma::TRANSFORMER_SIZE; ++i) {
        EXPECT_EQ(pb.swap(i), i) << "Default PlugBoard should map " << i << " to itself";
    }
}

/** @brief Verifies custom plug pairs create correct wiring. */
TEST_F(PlugBoardTests, CustomConfiguration) {
    auto pairs = createPairs({{0, 3}, {1, 4}});
    auto pbResult = PlugBoard::create(pairs);
    ASSERT_TRUE(pbResult.has_value());
    PlugBoard& pb = *pbResult;

    EXPECT_EQ(pb.swap(0), 3);
    EXPECT_EQ(pb.swap(3), 0);
    EXPECT_EQ(pb.swap(1), 4);
    EXPECT_EQ(pb.swap(4), 1);

    EXPECT_EQ(pb.swap(2), 2);
    EXPECT_EQ(pb.swap(5), 5);
}

/** @brief Verifies plugboard is reciprocal: swap(swap(x)) == x. */
TEST_F(PlugBoardTests, Reciprocity) {
    auto pairs = createPairs({{0, 25}, {10, 20}});
    auto pbResult = PlugBoard::create(pairs);
    ASSERT_TRUE(pbResult.has_value());
    PlugBoard& pb = *pbResult;

    for (int i = 0; i < enigma::TRANSFORMER_SIZE; ++i) {
        int swapped = pb.swap(i);
        int back = pb.swap(swapped);
        EXPECT_EQ(back, i) << "Reciprocity failed for input " << i;
    }
}

/** @brief Verifies error returned when conflicting plug pairs are used. */
TEST_F(PlugBoardTests, ConflictHandling) {
    auto pairs = createPairs({{0, 1}, {0, 2}});
    auto pbResult = PlugBoard::create(pairs);
    ASSERT_FALSE(pbResult.has_value());
    EXPECT_EQ(pbResult.error(), enigma::EnigmaError::PlugBoardPortConflict);
}

/** @brief Verifies self-loop pairs (A-A) are handled correctly. */
TEST_F(PlugBoardTests, SelfLoop) {
    auto pairs = createPairs({{0, 0}, {1, 1}});
    auto pbResult = PlugBoard::create(pairs);
    ASSERT_TRUE(pbResult.has_value());
    PlugBoard& pb = *pbResult;

    EXPECT_EQ(pb.swap(0), 0);
    EXPECT_EQ(pb.swap(1), 1);
}

/** @brief Verifies out-of-bounds indices return unchanged. */
TEST_F(PlugBoardTests, OutOfBounds) {
    PlugBoard pb;
    EXPECT_EQ(pb.swap(-1), -1);
    EXPECT_EQ(pb.swap(26), 26);
    EXPECT_EQ(pb.swap(100), 100);
}

/** @brief Verifies uninitialized pairs (-1, -1) are skipped. */
TEST_F(PlugBoardTests, UninitializedPairs) {
    auto pairs = createPairs({{0, 1}, {-1, -1}, {2, 3}, {-1, -1}});
    auto pbResult = PlugBoard::create(pairs);
    ASSERT_TRUE(pbResult.has_value());
    PlugBoard& pb = *pbResult;

    EXPECT_EQ(pb.swap(0), 1);
    EXPECT_EQ(pb.swap(1), 0);
    EXPECT_EQ(pb.swap(2), 3);
    EXPECT_EQ(pb.swap(3), 2);
}

/** @brief Verifies error returned for out-of-range port indices. */
TEST_F(PlugBoardTests, OutOfRangePortIndex) {
    auto pairs = createPairs({{0, 1}, {30, 5}});
    auto pbResult = PlugBoard::create(pairs);
    ASSERT_FALSE(pbResult.has_value());
    EXPECT_EQ(pbResult.error(), enigma::EnigmaError::PlugBoardPortOutOfRange);
}

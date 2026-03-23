#include <gtest/gtest.h>
#include "Transformer.hpp"

/** @brief Concrete implementation of Transformer for testing purposes. */
class ConcreteTransformer : public Transformer {
public:
    ConcreteTransformer() : Transformer() {}

    int transform(int position, bool reverse = false) const override {
        return reverse ? transformReverse(position) : transformForward(position);
    }
    int transformForward(int position) const override { return position; }
    int transformReverse(int position) const override { return position; }
    int rotate() override { return 0; }

    void testSetTransformValue(int row, int col, AlphabetIndex value) { setTransformValue(row, col, value); }
    AlphabetIndex testGetTransformValue(int row, int col) const { return getTransformValue(row, col); }
    void testFillTransformRow(int row, AlphabetIndex value) { fillTransformRow(row, value); }
    void testCopyTransformRow(int row, const std::array<AlphabetIndex, enigma::TRANSFORMER_SIZE>& values) {
        copyTransformRow(row, values);
    }
    const std::array<AlphabetIndex, enigma::TRANSFORMER_SIZE>& testGetTransformRow(int row) const {
        return getTransformRow(row);
    }
};

/** @brief Verifies Transformer base class default initialization. */
TEST(TransformerTests, DefaultInitialization) {
    ConcreteTransformer transformer;
    EXPECT_EQ(transformer.getType(), TransformerType::Undefined);
}

/** @brief Verifies lookup table size is correctly initialized. */
TEST(TransformerTests, LUTSize) {
    ConcreteTransformer transformer;
    EXPECT_EQ(transformer.sizeOfLookupTable(), 52);
}

/** @brief Verifies setTransformValue and getTransformValue work correctly. */
TEST(TransformerTests, SetAndGetTransformValue) {
    ConcreteTransformer transformer;
    transformer.testSetTransformValue(0, 5, 10);
    EXPECT_EQ(transformer.testGetTransformValue(0, 5), 10);
    transformer.testSetTransformValue(1, 15, 20);
    EXPECT_EQ(transformer.testGetTransformValue(1, 15), 20);
}

/** @brief Verifies fillTransformRow fills entire row with specified value. */
TEST(TransformerTests, FillTransformRow) {
    ConcreteTransformer transformer;
    transformer.testFillTransformRow(0, 99);
    for (int i = 0; i < enigma::TRANSFORMER_SIZE; ++i) {
        EXPECT_EQ(transformer.testGetTransformValue(0, i), 99);
    }
}

/** @brief Verifies copyTransformRow copies array values correctly. */
TEST(TransformerTests, CopyTransformRow) {
    ConcreteTransformer transformer;
    std::array<AlphabetIndex, enigma::TRANSFORMER_SIZE> values;
    for (int i = 0; i < enigma::TRANSFORMER_SIZE; ++i) {
        values[i] = static_cast<AlphabetIndex>(i * 2);
    }
    transformer.testCopyTransformRow(1, values);
    for (int i = 0; i < enigma::TRANSFORMER_SIZE; ++i) {
        EXPECT_EQ(transformer.testGetTransformValue(1, i), i * 2);
    }
}

/** @brief Verifies getTransformRow returns correct row reference. */
TEST(TransformerTests, GetTransformRow) {
    ConcreteTransformer transformer;
    std::array<AlphabetIndex, enigma::TRANSFORMER_SIZE> values;
    for (int i = 0; i < enigma::TRANSFORMER_SIZE; ++i) {
        values[i] = static_cast<AlphabetIndex>(i);
    }
    transformer.testCopyTransformRow(0, values);
    const auto& row = transformer.testGetTransformRow(0);
    EXPECT_EQ(row[0], 0);
    EXPECT_EQ(row[25], 25);
}

#include <gtest/gtest.h>
#include "Transformer.hpp"

/**
 * @brief Concrete implementation of Transformer for testing purposes.
 */
class ConcreteTransformer : public Transformer {
public:
    ConcreteTransformer() : Transformer() {}

    // Minimal implementations of pure virtual methods
    int transform(int position, bool /*reverse*/ = false) override { return position; }
    int rotate() override { return 0; }
};

TEST(TransformerTests, DefaultInitialization) {
    ConcreteTransformer transformer;
    EXPECT_EQ(transformer.getType(), TransformerType::Undefined);
}

TEST(TransformerTests, LUTSize) {
    ConcreteTransformer transformer;
    // TRANSFORMER_SIZE is 26, LUT is [2][26] = 52 elements
    EXPECT_EQ(transformer.sizeOfLookupTable(), 52);
}
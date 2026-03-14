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

// Copyright (c) 2025 Maxim [maxirmx] Samsonov
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// This file is a part of tip5xx library

#include <gtest/gtest.h>
#include <random>
#include <unordered_set>
#include "tip5xx/b_field_element.hpp"
#include "random_generator.hpp"

using namespace tip5xx;

// Small field elements test (converted from proptest)
TEST(BFieldElementTest, ByteArrayOfSmallFieldElementsIsZeroAtHighIndices) {
    // Test for all possible u8 values
    for (uint16_t val = 0; val <= 255; val++) {
        uint8_t value = static_cast<uint8_t>(val);

        // Create BFieldElement from the u8 value
        BFieldElement bfe = BFieldElement::new_element(static_cast<uint64_t>(value));

        // Convert to byte array
        std::array<uint8_t, 8> byte_array = bfe.raw_bytes();

        // First byte should equal the input value
        EXPECT_EQ(value, byte_array[0]) << "Failed for value " << val;

        // All other bytes should be zero
        for (size_t i = 1; i < 8; i++) {
            EXPECT_EQ(0, byte_array[i]) << "Byte " << i << " not zero for value " << val;
        }
    }
}

// Byte array conversion test (converted from proptest)
TEST(BFieldElementTest, ByteArrayConversion) {
    RandomGenerator rng;

    // Run 100 random tests
    for (int i = 0; i < 100; i++) {
        BFieldElement bfe = rng.random_bfe();

        std::array<uint8_t, 8> array = bfe.raw_bytes();
        BFieldElement bfe_recalculated = BFieldElement::from_raw_bytes(array);

        EXPECT_EQ(bfe, bfe_recalculated) << "Failed on iteration " << i;
    }
}

// Byte array outside range test (converted from proptest)
TEST(BFieldElementTest, ByteArrayOutsideRangeIsNotAccepted) {
    RandomGenerator rng;

    // Run multiple tests with values outside the field range
    for (int i = 0; i < 100; i++) {
        uint64_t value = BFieldElement::P + rng.random_range(0, 1000);
        std::array<uint8_t, 8> byte_array;

        // Convert to little-endian bytes
        for (int j = 0; j < 8; j++) {
            byte_array[j] = static_cast<uint8_t>(value & 0xFF);
            value >>= 8;
        }

        // Try converting - this should throw an exception
        try {
            [[maybe_unused]] BFieldElement bfe = BFieldElement::from_raw_bytes(byte_array);
            FAIL() << "Should have rejected value outside range";
        } catch (const ParseBFieldElementError& e) {
            // Expected behavior
            EXPECT_EQ(e.type(), ParseBFieldElementError::ErrorType::NotCanonical);
            SUCCEED();
        }
    }
}

// One is one test (converted from test)
TEST(BFieldElementTest, OneIsOne) {
    BFieldElement one = BFieldElement::ONE;
    ASSERT_TRUE(one.is_one());
    ASSERT_EQ(one, BFieldElement::ONE);
}

// Not one is not one test (converted from proptest)
TEST(BFieldElementTest, NotOneIsNotOne) {
    RandomGenerator rng;

    for (int i = 0; i < 100; i++) {
        BFieldElement bfe = rng.random_bfe();

        // Skip if the value is 1
        if (bfe.value() == 1) {
            continue;
        }

        EXPECT_FALSE(bfe.is_one()) << "Value " << bfe.value() << " should not be one";
    }
}

// One unequal zero test (converted from test)
TEST(BFieldElementTest, OneUnequalZero) {
    BFieldElement one = BFieldElement::ONE;
    BFieldElement zero = BFieldElement::ZERO;
    ASSERT_NE(one, zero);
}

// Value preservation test (converted from proptest)
TEST(BFieldElementTest, ValueIsPreserved) {
    RandomGenerator rng;

    for (int i = 0; i < 100; i++) {
        uint64_t value = rng.random_range(BFieldElement::P - 1);
        EXPECT_EQ(value, BFieldElement::new_element(value).value());
    }
}

// Zero is zero test (converted from test)
TEST(BFieldElementTest, ZeroIsZero) {
    BFieldElement zero = BFieldElement::ZERO;
    ASSERT_TRUE(zero.is_zero());
    ASSERT_EQ(zero, BFieldElement::ZERO);
}

// Not zero is nonzero test (converted from proptest)
TEST(BFieldElementTest, NotZeroIsNonzero) {
    RandomGenerator rng;

    for (int i = 0; i < 100; i++) {
        BFieldElement bfe = rng.random_bfe();

        // Skip if the value is 0
        if (bfe.value() == 0) {
            continue;
        }

        EXPECT_FALSE(bfe.is_zero()) << "Value " << bfe.value() << " should not be zero";
    }
}

// Addition is commutative test (converted from proptest)
TEST(BFieldElementTest, AdditionIsCommutative) {
    RandomGenerator rng;

    for (int i = 0; i < 100; i++) {
        BFieldElement element_0 = rng.random_bfe();
        BFieldElement element_1 = rng.random_bfe();

        EXPECT_EQ(element_0 + element_1, element_1 + element_0);
    }
}

// Multiplication is commutative test (converted from proptest)
TEST(BFieldElementTest, MultiplicationIsCommutative) {
    RandomGenerator rng;

    for (int i = 0; i < 100; i++) {
        BFieldElement element_0 = rng.random_bfe();
        BFieldElement element_1 = rng.random_bfe();

        EXPECT_EQ(element_0 * element_1, element_1 * element_0);
    }
}

// Addition is associative test (converted from proptest)
TEST(BFieldElementTest, AdditionIsAssociative) {
    RandomGenerator rng;

    for (int i = 0; i < 100; i++) {
        BFieldElement element_0 = rng.random_bfe();
        BFieldElement element_1 = rng.random_bfe();
        BFieldElement element_2 = rng.random_bfe();

        EXPECT_EQ(
            (element_0 + element_1) + element_2,
            element_0 + (element_1 + element_2)
        );
    }
}

// Multiplication is associative test (converted from proptest)
TEST(BFieldElementTest, MultiplicationIsAssociative) {
    RandomGenerator rng;

    for (int i = 0; i < 100; i++) {
        BFieldElement element_0 = rng.random_bfe();
        BFieldElement element_1 = rng.random_bfe();
        BFieldElement element_2 = rng.random_bfe();

        EXPECT_EQ(
            (element_0 * element_1) * element_2,
            element_0 * (element_1 * element_2)
        );
    }
}

// Multiplication distributes over addition test (converted from proptest)
TEST(BFieldElementTest, MultiplicationDistributesOverAddition) {
    RandomGenerator rng;

    for (int i = 0; i < 100; i++) {
        BFieldElement element_0 = rng.random_bfe();
        BFieldElement element_1 = rng.random_bfe();
        BFieldElement element_2 = rng.random_bfe();

        EXPECT_EQ(
            element_0 * (element_1 + element_2),
            element_0 * element_1 + element_0 * element_2
        );
    }
}

// Multiplication with inverse gives identity test (converted from proptest)
TEST(BFieldElementTest, MultiplicationWithInverseGivesIdentity) {
    RandomGenerator rng;

    for (int i = 0; i < 100; i++) {
        BFieldElement bfe = rng.random_bfe();

        // Skip if the value is 0
        if (bfe.is_zero()) {
            continue;
        }

        EXPECT_TRUE((bfe.inverse() * bfe).is_one());
    }
}

// Division by self gives identity test (converted from proptest)
TEST(BFieldElementTest, DivisionBySelfGivesIdentity) {
    RandomGenerator rng;

    for (int i = 0; i < 100; i++) {
        BFieldElement bfe = rng.random_bfe();

        // Skip if the value is 0
        if (bfe.is_zero()) {
            continue;
        }

        EXPECT_TRUE((bfe / bfe).is_one());
    }
}

// Values larger than modulus are handled correctly test (converted from proptest)
TEST(BFieldElementTest, ValuesLargerThanModulusAreHandledCorrectly) {
    RandomGenerator rng;

    for (int i = 0; i < 100; i++) {
        uint64_t large_value = BFieldElement::P + rng.random_range(1000);
        BFieldElement bfe = BFieldElement::new_element(large_value);
        uint64_t expected_value = large_value - BFieldElement::P;

        EXPECT_EQ(expected_value, bfe.value());
    }
}

// Zero is neutral element for addition test (converted from proptest)
TEST(BFieldElementTest, ZeroIsNeutralElementForAddition) {
    RandomGenerator rng;

    for (int i = 0; i < 100; i++) {
        BFieldElement bfe = rng.random_bfe();
        BFieldElement zero = BFieldElement::ZERO;

        EXPECT_EQ(bfe + zero, bfe);
    }
}

// One is neutral element for multiplication test (converted from proptest)
TEST(BFieldElementTest, OneIsNeutralElementForMultiplication) {
    RandomGenerator rng;

    for (int i = 0; i < 100; i++) {
        BFieldElement bfe = rng.random_bfe();
        BFieldElement one = BFieldElement::ONE;

        EXPECT_EQ(bfe * one, bfe);
    }
}

// Increment test (converted from proptest)
TEST(BFieldElementTest, Increment) {
    RandomGenerator rng;

    for (int i = 0; i < 100; i++) {
        BFieldElement bfe = rng.random_bfe();
        uint64_t old_value = bfe.value();

        bfe.increment();
        uint64_t expected_value = (old_value + 1) % BFieldElement::P;

        EXPECT_EQ(expected_value, bfe.value());
    }
}

// Incrementing max value wraps around test (converted from test)
TEST(BFieldElementTest, IncrementingMaxValueWrapsAround) {
    BFieldElement bfe = BFieldElement::new_element(BFieldElement::MAX);
    bfe.increment();
    ASSERT_EQ(0, bfe.value());
}

// Decrement test (converted from proptest)
TEST(BFieldElementTest, Decrement) {
    RandomGenerator rng;

    for (int i = 0; i < 100; i++) {
        BFieldElement bfe = rng.random_bfe();
        uint64_t old_value = bfe.value();

        bfe.decrement();
        uint64_t expected_value = (old_value == 0) ? BFieldElement::P - 1 : old_value - 1;

        EXPECT_EQ(expected_value, bfe.value());
    }
}

// Decrementing min value wraps around test (converted from test)
TEST(BFieldElementTest, DecrementingMinValueWrapsAround) {
    BFieldElement bfe = BFieldElement::ZERO;
    bfe.decrement();
    ASSERT_EQ(BFieldElement::MAX, bfe.value());
}

// Empty batch inversion test (converted from test)
TEST(BFieldElementTest, EmptyBatchInversion) {
    std::vector<BFieldElement> empty_inv = BFieldElement::batch_inversion({});
    ASSERT_TRUE(empty_inv.empty());
}

// Batch inversion test (converted from proptest)
TEST(BFieldElementTest, BatchInversion) {
    RandomGenerator rng;

    // Test different sized batches
    for (int size = 1; size < 20; size++) {
        std::vector<BFieldElement> bfes = rng.random_elements(size);
        std::vector<BFieldElement> bfes_inv = BFieldElement::batch_inversion(bfes);

        EXPECT_EQ(bfes.size(), bfes_inv.size());

        for (size_t i = 0; i < bfes.size(); i++) {
            // Skip zero elements
            if (bfes[i].is_zero()) {
                continue;
            }

            EXPECT_TRUE((bfes[i] * bfes_inv[i]).is_one())
                << "Failed at index " << i << " with value " << bfes[i].value();
        }
    }
}

// Multiplicative inverse of zero test (converted from test)
TEST(BFieldElementTest, MultiplicativeInverseOfZero) {
    BFieldElement zero = BFieldElement::ZERO;

    // Should throw an exception
    EXPECT_THROW(zero.inverse(), BFieldElementInverseError);
}

// Supposed generator is generator test (converted from test)
TEST(BFieldElementTest, SupposedGeneratorIsGenerator) {
    BFieldElement generator = BFieldElement::generator();
    uint64_t largest_meaningful_power = BFieldElement::P - 1;
    BFieldElement generator_pow_p = generator.mod_pow(largest_meaningful_power);
    BFieldElement generator_pow_p_half = generator.mod_pow(largest_meaningful_power / 2);

    ASSERT_EQ(BFieldElement::ONE, generator_pow_p);
    ASSERT_NE(BFieldElement::ONE, generator_pow_p_half);
}

// Test that gets more coverage than the original property tests
TEST(BFieldElementTest, AddSubWrapAroundTest) {
    // Test overflow behaviors
    BFieldElement element = BFieldElement::new_element(4);
    BFieldElement sum = BFieldElement::new_element(BFieldElement::MAX) + element;
    ASSERT_EQ(BFieldElement::new_element(3), sum);

    BFieldElement diff = sum - element;
    ASSERT_EQ(BFieldElement::new_element(BFieldElement::MAX), diff);
}

TEST(BFieldElementTest, NegTest) {
    // Test negation
    ASSERT_EQ(-BFieldElement::ZERO, BFieldElement::ZERO);
    ASSERT_EQ((-BFieldElement::ONE).value(), BFieldElement::MAX);

    BFieldElement max = BFieldElement::new_element(BFieldElement::MAX);
    BFieldElement max_plus_one = max + BFieldElement::ONE;
    BFieldElement max_plus_two = max_plus_one + BFieldElement::ONE;

    ASSERT_EQ(BFieldElement::ZERO, -max_plus_one);
    ASSERT_EQ(max, -max_plus_two);
}

// Mul div plus minus neg property test
TEST(BFieldElementTest, MulDivPlusMinusNeg) {
    RandomGenerator rng;
    std::vector<BFieldElement> elements = rng.random_elements(300);

    std::array<BFieldElement, 6> power_input_b;
    for (int i = 0; i < 6; i++) {
        power_input_b[i] = rng.random_bfe();
    }

    for (size_t i = 1; i < elements.size(); i++) {
        BFieldElement a = elements[i - 1];
        BFieldElement b = elements[i];

        BFieldElement ab = a * b;
        BFieldElement a_o_b = a / b;
        BFieldElement b_o_a = b / a;

        EXPECT_EQ(a, ab / b);
        EXPECT_EQ(b, ab / a);
        EXPECT_EQ(a, a_o_b * b);
        EXPECT_EQ(b, b_o_a * a);
        EXPECT_TRUE((a_o_b * b_o_a).is_one());
        BFieldElement square_result = a * a;
        EXPECT_EQ(a.square(), square_result);

        EXPECT_EQ(a - b + b, a);
        EXPECT_EQ(b - a + a, b);
        EXPECT_TRUE((a - a).is_zero());
        EXPECT_TRUE((b - b).is_zero());

        // Test the add/sub/mul assign operators
        BFieldElement a_minus_b = a;
        a_minus_b -= b;
        EXPECT_EQ(a - b, a_minus_b);

        BFieldElement a_plus_b = a;
        a_plus_b += b;
        EXPECT_EQ(a + b, a_plus_b);

        BFieldElement a_mul_b = a;
        a_mul_b *= b;
        EXPECT_EQ(a * b, a_mul_b);
        EXPECT_EQ(b * a, a_mul_b);

        // Test negation
        EXPECT_TRUE((-a + a).is_zero());
        EXPECT_TRUE((-b + b).is_zero());
        EXPECT_TRUE((-ab + ab).is_zero());
        EXPECT_TRUE((-a_o_b + a_o_b).is_zero());
        EXPECT_TRUE((-b_o_a + b_o_a).is_zero());
        EXPECT_TRUE((-a_minus_b + a_minus_b).is_zero());
        EXPECT_TRUE((-a_plus_b + a_plus_b).is_zero());
        EXPECT_TRUE((-a_mul_b + a_mul_b).is_zero());
    }
}

// Test mul_div_pbt
TEST(BFieldElementTest, MulDivPropertyBasedTest) {
    // Verify that multiplication results are consistent
    RandomGenerator rng;
    std::vector<BFieldElement> rands = rng.random_elements(100);

    for (size_t i = 1; i < rands.size(); i++) {
        BFieldElement prod_mul = rands[i - 1] * rands[i];
        BFieldElement prod_mul_assign = rands[i - 1];
        prod_mul_assign *= rands[i];

        EXPECT_EQ(prod_mul, prod_mul_assign)
            << "mul and mul_assign must be the same for B field elements";

        EXPECT_EQ(prod_mul / rands[i - 1], rands[i]);
        EXPECT_EQ(prod_mul / rands[i], rands[i - 1]);
    }
}

// Test neg_test
TEST(BFieldElementTest, NegationTest) {
    EXPECT_EQ(-BFieldElement::ZERO, BFieldElement::ZERO);
    EXPECT_EQ((-BFieldElement::ONE).value(), BFieldElement::MAX);

    BFieldElement max = BFieldElement::new_element(BFieldElement::MAX);
    BFieldElement max_plus_one = max + BFieldElement::ONE;
    BFieldElement max_plus_two = max_plus_one + BFieldElement::ONE;

    EXPECT_EQ(BFieldElement::ZERO, -max_plus_one);
    EXPECT_EQ(max, -max_plus_two);
}

// Test equality_and_hash_test
TEST(BFieldElementTest, EqualityAndHashTest) {
    EXPECT_EQ(BFieldElement::ZERO, BFieldElement::ZERO);
    EXPECT_EQ(BFieldElement::ONE, BFieldElement::ONE);
    EXPECT_NE(BFieldElement::ONE, BFieldElement::ZERO);
    EXPECT_EQ(BFieldElement::new_element(42), BFieldElement::new_element(42));
    EXPECT_NE(BFieldElement::new_element(42), BFieldElement::new_element(43));

    BFieldElement a = BFieldElement::new_element(102);
    BFieldElement b = BFieldElement::new_element(BFieldElement::MAX) + BFieldElement::new_element(103);
    EXPECT_EQ(a, b);
    EXPECT_NE(BFieldElement::new_element(103), b);

    // Use a custom hash function instead of depending on std::hash specialization
    auto custom_hash = [](const BFieldElement& bfe) {
        return std::hash<uint64_t>{}(bfe.value());
    };

    EXPECT_EQ(custom_hash(BFieldElement::new_element(42)), custom_hash(BFieldElement::new_element(42)));
    EXPECT_EQ(custom_hash(b), custom_hash(BFieldElement::new_element(102)));
}

// Test create_polynomial_test (assuming we have a Polynomial class)
//TEST(BFieldElementTest, CreatePolynomialTest) {
    // This would depend on having a Polynomial class implementation
    // Adjust according to your C++ implementation
//    Polynomial<BFieldElement> a({bfe(1), bfe(3), bfe(7)});
//    Polynomial<BFieldElement> b({bfe(2), bfe(5), bfe(-1)});
//    Polynomial<BFieldElement> expected({bfe(3), bfe(8), bfe(6)});
//
//    EXPECT_EQ(expected, a + b);
//}

// Test mod_pow_test_powers_of_two
TEST(BFieldElementTest, ModPowPowersOfTwo) {
    BFieldElement two = BFieldElement::new_element(2);
    // 2^63 < 2^64, so no wrap-around of B-field element
    for (uint64_t i = 0; i < 64; i++) {
        EXPECT_EQ(BFieldElement::new_element(1ULL << i), two.mod_pow(i))
            << "Failed for power: " << i;
    }
}

// Test mod_pow_test_powers_of_three
TEST(BFieldElementTest, ModPowPowersOfThree) {
    BFieldElement three = BFieldElement::new_element(3);
    // 3^40 < 2^64, so no wrap-around of B-field element
    for (uint64_t i = 0; i < 41; i++) {
        uint64_t expected = 1;
        for (uint64_t j = 0; j < i; j++) {
            expected *= 3;
        }
        EXPECT_EQ(BFieldElement::new_element(expected), three.mod_pow(i))
            << "Failed for power: " << i;
    }
}

// Test mod_pow_test
TEST(BFieldElementTest, ModPowTest) {
    // Test specific primitive roots
    EXPECT_TRUE(BFieldElement::new_element(281474976710656ULL).mod_pow(4).is_one());
    EXPECT_EQ(
        BFieldElement::new_element(281474976710656ULL),
        BFieldElement::new_element(281474976710656ULL).mod_pow(5)
    );
    EXPECT_TRUE(BFieldElement::new_element(18446744069414584320ULL).mod_pow(2).is_one());
    EXPECT_TRUE(BFieldElement::new_element(18446744069397807105ULL).mod_pow(8).is_one());
    EXPECT_TRUE(BFieldElement::new_element(2625919085333925275ULL).mod_pow(10).is_one());
    EXPECT_TRUE(BFieldElement::new_element(281474976645120ULL).mod_pow(12).is_one());
    EXPECT_TRUE(BFieldElement::new_element(0).mod_pow(0).is_one());
}

// Test u32_conversion
TEST(BFieldElementTest, U32Conversion) {
    BFieldElement val = BFieldElement::new_element(UINT32_MAX);
    uint32_t as_u32 = 0;
    ASSERT_NO_THROW(as_u32 = static_cast<uint32_t>(val));
    EXPECT_EQ(UINT32_MAX, as_u32);

    for (uint64_t i = 1; i < 100; i++) {
        BFieldElement invalid_val = BFieldElement::new_element(static_cast<uint64_t>(UINT32_MAX) + i);
        EXPECT_THROW((void)static_cast<uint32_t>(invalid_val), BFieldElementStringConversionError);
    }
}

// Test inverse_or_zero_bfe
TEST(BFieldElementTest, InverseOrZero) {
    BFieldElement zero = BFieldElement::ZERO;
    BFieldElement one = BFieldElement::ONE;

    EXPECT_EQ(zero, zero.inverse_or_zero());

    // Test with random elements
    RandomGenerator rng;
    for (int i = 0; i < 100; i++) {
        BFieldElement elem = rng.random_bfe();
        if (elem.is_zero()) {
            EXPECT_EQ(zero, elem.inverse_or_zero());
        } else {
            EXPECT_EQ(one, elem * elem.inverse_or_zero());
        }
    }
}

// Test random_squares
TEST(BFieldElementTest, RandomSquares) {
    RandomGenerator rng;
    uint64_t p = BFieldElement::P;

    for (int i = 0; i < 100; i++) {
        uint64_t a = rng.random_range(p - 1);
        uint64_t asq = (static_cast<__uint128_t>(a) * static_cast<__uint128_t>(a) % p);

        BFieldElement b = BFieldElement::new_element(a);
        BFieldElement bsq = BFieldElement::new_element(asq);

        EXPECT_EQ(bsq, b * b);
        EXPECT_EQ(bsq.value(), (b * b).value());
        EXPECT_EQ(b.value(), a);
        EXPECT_EQ(bsq.value(), asq);
    }

    BFieldElement one = BFieldElement::new_element(1);
    EXPECT_EQ(one, one * one);
}

// Test equals
TEST(BFieldElementTest, Equals) {
    BFieldElement a = BFieldElement::ONE;
    BFieldElement b = BFieldElement::new_element(BFieldElement::MAX) *
                     BFieldElement::new_element(BFieldElement::MAX);

    // Elements should be equal
    EXPECT_EQ(a, b);
    EXPECT_EQ(a.value(), b.value());
}

// Test raw byte and chunk conversion methods
TEST(BFieldElementTest, TestRandomRaw) {
    RandomGenerator rng;

    // Test 100 random elements
    for (int i = 0; i < 100; i++) {
        // Generate a random BFieldElement
        BFieldElement e = rng.random_bfe();

        // Test byte conversion round trip
        std::array<uint8_t, 8> bytes = e.raw_bytes();
        BFieldElement c = BFieldElement::from_raw_bytes(bytes);
        ASSERT_EQ(e, c) << "Byte conversion round trip failed";

        // Manually reconstruct from bytes
        uint64_t f = 0;
        for (size_t i = 0; i < bytes.size(); i++) {
            f += static_cast<uint64_t>(bytes[i]) << (8 * i);
        }
        ASSERT_EQ(e, BFieldElement::new_element(f)) << "Manual byte reconstruction failed";

        // Test u16 chunk conversion round trip
        std::array<uint16_t, 4> chunks = e.raw_u16s();
        BFieldElement g = BFieldElement::from_raw_u16s(chunks);
        ASSERT_EQ(e, g) << "u16 chunk conversion round trip failed";

        // Manually reconstruct from u16 chunks
        uint64_t h = 0;
        for (size_t i = 0; i < chunks.size(); i++) {
            h += static_cast<uint64_t>(chunks[i]) << (16 * i);
        }
        BFieldElement n = BFieldElement::new_element(h);
        ASSERT_EQ(e, n) << "Manual u16 chunk reconstruction failed";
    }
}

// Test fixed_inverse
TEST(BFieldElementTest, FixedInverse) {
    // Test specific inverse case
    BFieldElement a = BFieldElement::new_element(8561862112314395584);
    BFieldElement a_inv = a.inverse();
    BFieldElement a_inv_or_0 = a.inverse_or_zero();
    BFieldElement expected = BFieldElement::new_element(17307602810081694772ULL);

    EXPECT_EQ(a_inv, a_inv_or_0);
    EXPECT_EQ(a_inv, expected);
}

// Test fixed_modpow
TEST(BFieldElementTest, FixedModPow) {
    uint64_t exponent = 16608971246357572739ULL;
    BFieldElement base = BFieldElement::new_element(7808276826625786800);
    BFieldElement expected = BFieldElement::new_element(2288673415394035783);

    EXPECT_EQ(base.mod_pow_u64(exponent), expected);
    EXPECT_EQ(base.mod_pow_u64_impl(exponent), expected);
}

// Test fixed_mul
TEST(BFieldElementTest, FixedMul) {
    {
        BFieldElement a = BFieldElement::new_element(2779336007265862836);
        BFieldElement b = BFieldElement::new_element(8146517303801474933);
        BFieldElement c = a * b;
        BFieldElement expected = BFieldElement::new_element(1857758653037316764);
        EXPECT_EQ(c, expected);
    }

    {
        BFieldElement a = BFieldElement::new_element(9223372036854775808ULL);
        BFieldElement b = BFieldElement::new_element(9223372036854775808ULL);
        BFieldElement c = a * b;
        BFieldElement expected = BFieldElement::new_element(18446744068340842497ULL);
        EXPECT_EQ(c, expected);
    }
}

// Test conversion_from_i32_to_bfe_is_correct
TEST(BFieldElementTest, ConversionFromI32ToBfe) {
    RandomGenerator rng;

    for (int i = 0; i < 100; i++) {
        int32_t v = static_cast<int32_t>(rng.random_range(INT32_MIN, INT32_MAX));
        BFieldElement bfe = bfe_from(v);

        if (v >= 0) {
            EXPECT_EQ(static_cast<uint64_t>(v), bfe.value());
        } else {
            EXPECT_EQ(static_cast<uint64_t>(-v), BFieldElement::P - bfe.value());
        }
    }
}

// Test conversion_from_isize_to_bfe_is_correct
TEST(BFieldElementTest, ConversionFromISizeToBfe) {
    RandomGenerator rng;

    for (int i = 0; i < 100; i++) {
        int64_t v = static_cast<int64_t>(rng.random_range(INT32_MIN, INT32_MAX));
        BFieldElement bfe = bfe_from(v);

        if (v >= 0) {
            EXPECT_EQ(static_cast<uint64_t>(v), bfe.value());
        } else {
            EXPECT_EQ(static_cast<uint64_t>(-v), BFieldElement::P - bfe.value());
        }
    }
}

// Test various type conversions
TEST(BFieldElementTest, MultipleTypeConversions) {
    // Test various integer type conversions to BFieldElement
    EXPECT_NO_THROW(bfe_from(static_cast<uint8_t>(0)));
    EXPECT_NO_THROW(bfe_from(static_cast<uint16_t>(0)));
    EXPECT_NO_THROW(bfe_from(static_cast<uint32_t>(0)));
    EXPECT_NO_THROW(bfe_from(static_cast<uint64_t>(0)));
    EXPECT_NO_THROW(bfe_from(static_cast<size_t>(0)));

    // Test negative values
    BFieldElement max = BFieldElement::new_element(BFieldElement::MAX);
    EXPECT_EQ(max, bfe_from(static_cast<int8_t>(-1)));
    EXPECT_EQ(max, bfe_from(static_cast<int16_t>(-1)));
    EXPECT_EQ(max, bfe_from(static_cast<int32_t>(-1)));
    EXPECT_EQ(max, bfe_from(static_cast<int64_t>(-1)));

    // Test conversions from BFieldElement to integer types
    EXPECT_NO_THROW((void)static_cast<uint8_t>(BFieldElement::ZERO));
    EXPECT_NO_THROW((void)static_cast<int8_t>(BFieldElement::ZERO));
    EXPECT_NO_THROW((void)static_cast<uint16_t>(BFieldElement::ZERO));
    EXPECT_NO_THROW((void)static_cast<int16_t>(BFieldElement::ZERO));
    EXPECT_NO_THROW((void)static_cast<uint32_t>(BFieldElement::ZERO));
    EXPECT_NO_THROW((void)static_cast<int32_t>(BFieldElement::ZERO));
    EXPECT_NO_THROW((void)static_cast<size_t>(BFieldElement::ZERO));

    // Test larger conversion
    EXPECT_NO_THROW((void)static_cast<uint64_t>(max));
    EXPECT_NO_THROW((void)static_cast<int64_t>(max));
}

// Test bfield_conversion_works_for_types_min_and_max
TEST(BFieldElementTest, ConversionForTypesMinAndMax) {
    // Test min/max conversions for various integer types
    EXPECT_NO_THROW(bfe_from(std::numeric_limits<uint8_t>::min()));
    EXPECT_NO_THROW(bfe_from(std::numeric_limits<uint8_t>::max()));
    EXPECT_NO_THROW(bfe_from(std::numeric_limits<uint16_t>::min()));
    EXPECT_NO_THROW(bfe_from(std::numeric_limits<uint16_t>::max()));
    EXPECT_NO_THROW(bfe_from(std::numeric_limits<uint32_t>::min()));
    EXPECT_NO_THROW(bfe_from(std::numeric_limits<uint32_t>::max()));
    EXPECT_NO_THROW(bfe_from(std::numeric_limits<uint64_t>::min()));
    EXPECT_NO_THROW(bfe_from(std::numeric_limits<uint64_t>::max()));
    EXPECT_NO_THROW(bfe_from(std::numeric_limits<size_t>::min()));
    EXPECT_NO_THROW(bfe_from(std::numeric_limits<size_t>::max()));
    EXPECT_NO_THROW(bfe_from(std::numeric_limits<int8_t>::min()));
    EXPECT_NO_THROW(bfe_from(std::numeric_limits<int8_t>::max()));
    EXPECT_NO_THROW(bfe_from(std::numeric_limits<int16_t>::min()));
    EXPECT_NO_THROW(bfe_from(std::numeric_limits<int16_t>::max()));
    EXPECT_NO_THROW(bfe_from(std::numeric_limits<int32_t>::min()));
    EXPECT_NO_THROW(bfe_from(std::numeric_limits<int32_t>::max()));
    EXPECT_NO_THROW(bfe_from(std::numeric_limits<int64_t>::min()));
    EXPECT_NO_THROW(bfe_from(std::numeric_limits<int64_t>::max()));
}

// Test naive_and_actual_conversion_from_u128_agree
TEST(BFieldElementTest, U128ConversionConsistency) {
    RandomGenerator rng;

    auto naive_conversion = [](unsigned __int128 x) -> BFieldElement {
        unsigned __int128 p = BFieldElement::P;
        uint64_t value = static_cast<uint64_t>(x % p);
        return BFieldElement::new_element(value);
    };

    for (int i = 0; i < 100; i++) {
        // Generate a random 128-bit value by combining two 64-bit values
        uint64_t high = rng.random_range(UINT64_MAX);
        uint64_t low = rng.random_range(UINT64_MAX);
        unsigned __int128 v = (static_cast<unsigned __int128>(high) << 64) | low;

        EXPECT_EQ(naive_conversion(v), bfe_from(v));
    }
}

// Test naive_and_actual_conversion_from_i64_agree
TEST(BFieldElementTest, I64ConversionConsistency) {
    RandomGenerator rng;

    // Naive conversion using modular arithmetic directly
    auto naive_conversion = [](int64_t x) -> BFieldElement {
        // Cast the field modulus to __int128_t to avoid overflow
        const __int128_t p = static_cast<__int128_t>(BFieldElement::P);

        // Convert the input to __int128_t
        __int128_t value_i128 = static_cast<__int128_t>(x);

        // Calculate remainder (equivalent to rem_euclid in Rust)
        __int128_t remainder = value_i128 % p;
        if (remainder < 0) {
            remainder += p;
        }

        // Create BFieldElement from the reduced value
        return BFieldElement::new_element(static_cast<uint64_t>(remainder));
    };

    // Test a range of values including edge cases
    std::vector<int64_t> test_values = {
        INT64_MIN, INT64_MIN + 1, -BFieldElement::P, -BFieldElement::P + 1,
        -1000, -42, -1, 0, 1, 42, 1000,
        INT64_MAX - 1, INT64_MAX
    };

    // And some random values
    for (int i = 0; i < 80; i++) {
        test_values.push_back(static_cast<int64_t>(rng.random_range(UINT64_MAX) - INT64_MAX));
    }

    // Test each value
    for (int64_t value : test_values) {
        BFieldElement naive = naive_conversion(value);
        BFieldElement actual = bfe_from(value); // Uses the actual conversion method

        EXPECT_EQ(naive, actual) << "Failed for value: " << value;
    }
}

// Test const_minus_two_inverse_is_really_minus_two_inverse
TEST(BFieldElementTest, MinusTwoInverseIsCorrect) {
    EXPECT_EQ(bfe_from(-2).inverse(), BFieldElement::MINUS_TWO_INVERSE);
}

// Test int64_t-specific bfe_from implementation
TEST(BFieldElementTest, Int64SpecificConversion) {
    // Test positive values
    EXPECT_EQ(BFieldElement::new_element(42), bfe_from(static_cast<int64_t>(42)));

    // Test negative values (including edge cases)
    EXPECT_EQ(BFieldElement::new_element(BFieldElement::P - 1), bfe_from(static_cast<int64_t>(-1)));

    // Test INT64_MIN
    BFieldElement int64_min_result = bfe_from(INT64_MIN);
    // Verify the result matches the formula: (uint64_t)INT64_MIN - R2
    BFieldElement expected = bfe_from(static_cast<__uint128_t>(INT64_MIN) - static_cast<__uint128_t>(BFieldElement::R2));
    EXPECT_EQ(expected, int64_min_result);

    // Test that the specialized implementation matches the template for non-negative values
    for (int64_t i = 0; i < 100; i++) {
        EXPECT_EQ(BFieldElement::new_element(i), bfe_from(i));
    }
}

// Test exponentiation with 32-bit exponents
TEST(BFieldElementTest, ModPowU32) {
    RandomGenerator rng;

    // Test basic exponentiation
    BFieldElement two = BFieldElement::new_element(2);
    BFieldElement five = BFieldElement::new_element(5);

    // Test exponentiation with small values
    EXPECT_EQ(BFieldElement::ONE, two.mod_pow_u32_impl(0));
    EXPECT_EQ(two, two.mod_pow_u32(1));
    EXPECT_EQ(BFieldElement::new_element(32), two.mod_pow_u32(5));
    EXPECT_EQ(BFieldElement::new_element(1024), two.mod_pow_u32_impl(10));
    EXPECT_EQ(BFieldElement::new_element(3125), five.mod_pow_u32(5));

    // Test Fermat's Little Theorem: a^(p-1) ≡ 1 (mod p) for any a ≠ 0
    // For 32-bit test, use smaller exponent that's still interesting
    uint32_t small_fermat_exp = 0xFFFF; // 2^16 - 1

    // Test with several random values
    for (int i = 0; i < 10; i++) {
        BFieldElement random_base = rng.random_bfe();
        if (!random_base.is_zero()) {
            // a^(p-1) mod p should equal 1
            BFieldElement result = random_base.mod_pow_u32(small_fermat_exp);
            BFieldElement expected = random_base.mod_pow_u64(small_fermat_exp);
            EXPECT_EQ(expected, result);
        }
    }

    // Test maximum 32-bit exponent
    BFieldElement random_base = rng.random_bfe();
    if (random_base.is_one() || random_base.is_zero()) {
        random_base = BFieldElement::new_element(123); // Use a fixed value if random is 0 or 1
    }

    uint32_t max_u32 = std::numeric_limits<uint32_t>::max();

    // Make sure u32 and u64 versions are consistent
    BFieldElement result_u32 = random_base.mod_pow_u32(max_u32);
    BFieldElement result_u64 = random_base.mod_pow_u64(max_u32);
    EXPECT_EQ(result_u64, result_u32);
}

// Test primitive_root_of_unity function
TEST(BFieldElementTest, PrimitiveRootOfUnity) {
    // Test valid cases (powers of 2 up to 2^32)
    const std::vector<uint64_t> valid_powers = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024,
                                               2048, 4096, 8192, 16384, 32768, 65536,
                                               4294967296}; // 2^32

    for (uint64_t power : valid_powers) {
        // Should not throw
        EXPECT_NO_THROW({
            BFieldElement root = BFieldElement::primitive_root_of_unity(power);

            // Verify properties of a primitive root of unity

            // 1. root^power should be 1
            EXPECT_TRUE(root.mod_pow(power).is_one())
                << "root^" << power << " is not 1";

            // 2. root^(power/2) should not be 1 (if power > 1)
            if (power > 1) {
                EXPECT_FALSE(root.mod_pow(power / 2).is_one())
                    << "root^(" << power << "/2) is 1, which shouldn't happen";
            }
        });
    }

    // Test invalid cases that should throw exceptions
    const std::vector<uint64_t> invalid_powers = {
        3, 6, 7, 9, 15, 17, 31, 33,  // Non-powers of 2
        8589934592,                   // 2^33 (too large)
        UINT64_MAX                    // Maximum possible value
    };

    for (uint64_t power : invalid_powers) {
        EXPECT_THROW( (void)BFieldElement::primitive_root_of_unity(power), BFieldElementPrimitiveRootError) << "Expected exception for n = " << power;
    }
}

// Test cyclic_group_elements method
TEST(BFieldElementTest, CyclicGroupElements) {
    // Test small groups
    for (uint64_t order : {2, 4, 8, 16}) {
        // Get a generator for this order
        BFieldElement generator = BFieldElement::primitive_root_of_unity(order);

        // Get all elements of the cyclic group
        std::vector<BFieldElement> elements = generator.cyclic_group_elements();

        // Verify size
        EXPECT_EQ(order, elements.size()) << "Group should have " << order << " elements";

        // Verify uniqueness (no duplicates)
        std::unordered_set<uint64_t> unique_values;
        for (const auto& element : elements) {
            unique_values.insert(element.value());
        }
        EXPECT_EQ(order, unique_values.size()) << "All elements should be unique";

        // Verify that each element raised to the power of 'order' equals 1
        for (const auto& element : elements) {
            EXPECT_TRUE(element.mod_pow(order).is_one())
                << "Element " << element.value() << " raised to power " << order
                << " should equal 1";
        }

        // Verify that the elements form powers of the generator
        for (size_t i = 0; i < elements.size(); i++) {
            EXPECT_EQ(elements[i], generator.mod_pow(i))
                << "Element at index " << i << " should equal generator^" << i;
        }
    }

    // Test max parameter
    BFieldElement generator = BFieldElement::primitive_root_of_unity(16);
    std::vector<BFieldElement> limited = generator.cyclic_group_elements(5);
    EXPECT_EQ(5, limited.size()) << "Should limit to 5 elements";

    // Test with zero
    BFieldElement zero = BFieldElement::ZERO;
    std::vector<BFieldElement> zero_group = zero.cyclic_group_elements();
    EXPECT_EQ(1, zero_group.size()) << "Zero should generate a group of size 1";
    EXPECT_EQ(BFieldElement::ZERO, zero_group[0]) << "Zero group should only contain zero";
}

// Test stream output operator for BFieldElement
TEST(BFieldElementTest, StreamOutputOperator) {
    // Test small positive values (≤ 256)
    {
        BFieldElement small = BFieldElement::new_element(42);
        std::stringstream ss;
        ss << small;
        EXPECT_EQ("42", ss.str());
    }

    // Test zero
    {
        std::stringstream ss;
        ss << BFieldElement::ZERO;
        EXPECT_EQ("0", ss.str());
    }

    // Test values near P (shown as negative)
    {
        BFieldElement near_p = BFieldElement::new_element(BFieldElement::P - 1);
        std::stringstream ss;
        ss << near_p;
        EXPECT_EQ("-1", ss.str());
    }

    {
        BFieldElement neg_100 = BFieldElement::new_element(BFieldElement::P - 100);
        std::stringstream ss;
        ss << neg_100;
        EXPECT_EQ("-100", ss.str());
    }

    // Test a value just below the negative cutoff
    {
        BFieldElement below_neg_cutoff = BFieldElement::new_element(BFieldElement::P - 257);
        std::stringstream ss;
        ss << below_neg_cutoff;
        std::stringstream expected;
        expected.width(20);
        expected.fill('0');
        expected << (BFieldElement::P - 257);
        EXPECT_EQ(expected.str(), ss.str());
    }

    // Test a value just above the positive cutoff
    {
        BFieldElement above_cutoff = BFieldElement::new_element(257);
        std::stringstream ss;
        ss << above_cutoff;
        std::stringstream expected;
        expected.width(20);
        expected.fill('0');
        expected << 257;
        EXPECT_EQ(expected.str(), ss.str());
    }

    // Test a large middle-range value
    {
        BFieldElement mid_range = BFieldElement::new_element(BFieldElement::P / 2);
        std::stringstream ss;
        ss << mid_range;
        std::stringstream expected;
        expected.width(20);
        expected.fill('0');
        expected << (BFieldElement::P / 2);
        EXPECT_EQ(expected.str(), ss.str());
    }
}

TEST(BFieldElementTest, BfeFromString) {
    // Test basic decimal values
    EXPECT_EQ(BFieldElement::new_element(0), bfe_from_string("0"));
    EXPECT_EQ(BFieldElement::new_element(1), bfe_from_string("1"));
    EXPECT_EQ(BFieldElement::new_element(42), bfe_from_string("42"));
    EXPECT_EQ(BFieldElement::new_element(18446744069414584320ULL - 1),
              bfe_from_string("18446744069414584319"));

    // Test negative values
    EXPECT_EQ(BFieldElement::new_element(BFieldElement::P - 1), bfe_from_string("-1"));
    EXPECT_EQ(BFieldElement::new_element(BFieldElement::P - 42), bfe_from_string("-42"));

    // Test sign handling
    EXPECT_EQ(BFieldElement::new_element(42), bfe_from_string("+42"));

    // Test whitespace handling
    EXPECT_EQ(bfe_from_string(" 42"), BFieldElement::new_element(42));
    EXPECT_EQ(bfe_from_string("42 "), BFieldElement::new_element(42));
    EXPECT_THROW((void)bfe_from_string(" "), BFieldElementStringConversionError);

    // Test invalid inputs
    EXPECT_THROW((void)bfe_from_string(""), BFieldElementStringConversionError);
    EXPECT_THROW((void)bfe_from_string("abc"), BFieldElementStringConversionError);
    EXPECT_THROW((void)bfe_from_string("123abc"), BFieldElementStringConversionError);
    EXPECT_THROW((void)bfe_from_string("42.5"), BFieldElementStringConversionError);

    // Test values exceeding field modulus
    EXPECT_THROW((void)bfe_from_string("18446744069414584321"), BFieldElementStringConversionError);

    // Test extremely negative values
    EXPECT_THROW((void)bfe_from_string("-18446744069414584322"), BFieldElementStringConversionError);

    // Test large values triggering overflow check
    const std::string large_value(40, '9');  // String with 40 9's, exceeds 2^126
    EXPECT_THROW((void)bfe_from_string(large_value), BFieldElementStringConversionError);
}

TEST(BFieldElementTest, BfeFromHexString) {
    // Test basic hex values
    EXPECT_EQ(BFieldElement::new_element(0), bfe_from_string("0x0"));
    EXPECT_EQ(BFieldElement::new_element(1), bfe_from_hex_string("0x1"));
    EXPECT_EQ(BFieldElement::new_element(42), bfe_from_hex_string("0x2A"));
    EXPECT_EQ(BFieldElement::new_element(42), bfe_from_hex_string("0x2a"));

    // Test without 0x prefix
    EXPECT_EQ(BFieldElement::new_element(42), bfe_from_hex_string("2A"));

    // Test mixed case
    EXPECT_EQ(BFieldElement::new_element(0xABCD), bfe_from_hex_string("0xAbCd"));

    // Test large values near field modulus
    EXPECT_EQ(BFieldElement::new_element(BFieldElement::P - 1),
              bfe_from_hex_string("0xFFFFFFFF00000000"));

    // Test max uint64_t value
    EXPECT_EQ(BFieldElement::new_element(0xFFFFFFFFFFFFFFFFULL),
              bfe_from_hex_string("0xFFFFFFFFFFFFFFFF"));

    // Test values exceeding field modulus (should wrap around)
    EXPECT_EQ(BFieldElement::new_element(0),
              bfe_from_hex_string("0xFFFFFFFF00000001"));
    EXPECT_EQ(BFieldElement::new_element(1),
              bfe_from_hex_string("0xFFFFFFFF00000002"));

    // Test invalid inputs
    EXPECT_THROW((void)bfe_from_hex_string(""), BFieldElementStringConversionError);
    EXPECT_THROW((void)bfe_from_hex_string("0x"), BFieldElementStringConversionError);
    EXPECT_THROW((void)bfe_from_hex_string("0xG"), BFieldElementStringConversionError);
    EXPECT_THROW((void)bfe_from_hex_string("0x12ZZ"), BFieldElementStringConversionError);

    // Test large values exceeding 127 bits
    // This is 128 bits of all 1s, should overflow
    const std::string large_hex = "0x" + std::string(32, 'F');
    EXPECT_THROW((void)bfe_from_hex_string(large_hex), BFieldElementStringConversionError);

    // Test edge cases for overflow detection
    std::string almost_overflow = "0x7" + std::string(31, 'F');  // 127 bits of 1, should be ok
    EXPECT_NO_THROW((void)bfe_from_hex_string(almost_overflow));

    std::string exact_overflow = "0x8" + std::string(31, '0');  // 1 in 128th bit position, should overflow
    EXPECT_THROW((void)bfe_from_hex_string(exact_overflow), BFieldElementStringConversionError);
}

// Test stream input operator for BFieldElement
TEST(BFieldElementTest, StreamInputOperator) {
    // Test basic input
    {
        std::stringstream ss("42");
        BFieldElement bfe;
        ss >> bfe;
        EXPECT_EQ(BFieldElement::new_element(42), bfe);
    }

    // Test negative value
    {
        std::stringstream ss("-123");
        BFieldElement bfe;
        ss >> bfe;
        EXPECT_EQ(BFieldElement::new_element(BFieldElement::P - 123), bfe);
    }

    // Test hex value
    {
        std::stringstream ss("0xABCD");
        BFieldElement bfe;
        ss >> bfe;
        EXPECT_EQ(BFieldElement::new_element(0xABCD), bfe);
    }

    // Test input followed by other data
    {
        std::stringstream ss("123 456");
        BFieldElement bfe1, bfe2;
        ss >> bfe1 >> bfe2;
        EXPECT_EQ(BFieldElement::new_element(123), bfe1);
        EXPECT_EQ(BFieldElement::new_element(456), bfe2);
    }

    // Test error handling
    {
        std::stringstream ss("abc");
        BFieldElement bfe;
        EXPECT_THROW(ss >> bfe, BFieldElementStringConversionError);
    }
}

// Test BFieldElement::to_string() method
TEST(BFieldElementTest, ToString) {
    // Test small positive values (≤ 256)
    EXPECT_EQ("0", BFieldElement::ZERO.to_string());
    EXPECT_EQ("1", BFieldElement::ONE.to_string());
    EXPECT_EQ("42", BFieldElement::new_element(42).to_string());
    EXPECT_EQ("256", BFieldElement::new_element(256).to_string());

    // Test negative values (values close to P)
    EXPECT_EQ("-1", BFieldElement::new_element(BFieldElement::P - 1).to_string());
    EXPECT_EQ("-42", BFieldElement::new_element(BFieldElement::P - 42).to_string());
    EXPECT_EQ("-256", BFieldElement::new_element(BFieldElement::P - 256).to_string());

    // Test boundary cases
    BFieldElement just_above_cutoff = BFieldElement::new_element(257);
    std::stringstream expected1;
    expected1.width(20);
    expected1.fill('0');
    expected1 << 257;
    EXPECT_EQ(expected1.str(), just_above_cutoff.to_string());

    BFieldElement just_below_neg_cutoff = BFieldElement::new_element(BFieldElement::P - 257);
    std::stringstream expected2;
    expected2.width(20);
    expected2.fill('0');
    expected2 << (BFieldElement::P - 257);
    EXPECT_EQ(expected2.str(), just_below_neg_cutoff.to_string());

    // Test mid-range values
    BFieldElement mid_range = BFieldElement::new_element(BFieldElement::P / 2);
    std::stringstream expected3;
    expected3.width(20);
    expected3.fill('0');
    expected3 << (BFieldElement::P / 2);
    EXPECT_EQ(expected3.str(), mid_range.to_string());

    // Test max value
    BFieldElement max_value = BFieldElement::new_element(BFieldElement::MAX);
    EXPECT_EQ("-1", max_value.to_string());

    // Verify consistency with stream operator
    for (uint64_t i = 0; i < 300; i++) {
        BFieldElement elem = BFieldElement::new_element(i);
        std::stringstream ss;
        ss << elem;
        EXPECT_EQ(ss.str(), elem.to_string());
    }

    // Also test some values near P
    for (uint64_t i = 1; i < 300; i++) {
        BFieldElement elem = BFieldElement::new_element(BFieldElement::P - i);
        std::stringstream ss;
        ss << elem;
        EXPECT_EQ(ss.str(), elem.to_string());
    }
}

// Test handling of values exceeding uint64_t in bfe_from_string
TEST(BFieldElementTest, StringParsingUint64Overflow) {
    // Test the boundary condition for negative values
    std::string almost_overflow_neg = "-18446744069414584320";  // -P+1, should be fine
    EXPECT_NO_THROW((void)bfe_from_string(almost_overflow_neg));

    // Test boundary for positive values
    std::string max_allowed = "18446744069414584321";  // P, should fail as too large
    EXPECT_THROW((void)bfe_from_string(max_allowed), BFieldElementStringConversionError);

    std::string almost_max = "18446744069414584320";  // P-1, should be fine
    EXPECT_NO_THROW((void)bfe_from_string(almost_max));
}

// Test BFieldElement to<T> conversion with overflow/underflow handling
TEST(BFieldElementTest, ToTemplateExceptions) {
    // Test overflow for unsigned types
    {
        // Create a value just above uint8_t::max
        BFieldElement too_large = BFieldElement::new_element(256);
        EXPECT_THROW((void)too_large.to<uint8_t>(), BFieldElementStringConversionError);

        // Test at exact boundary
        BFieldElement at_max = BFieldElement::new_element(255);
        EXPECT_NO_THROW((void)at_max.to<uint8_t>());
        EXPECT_EQ(255, at_max.to<uint8_t>());
    }

    // Test overflow for signed types (positive too large)
    {
        // Create a value just above int8_t::max
        BFieldElement too_large = BFieldElement::new_element(128);
        EXPECT_THROW((void)too_large.to<int8_t>(), BFieldElementStringConversionError);

        // Test at exact boundary
        BFieldElement at_max = BFieldElement::new_element(127);
        EXPECT_NO_THROW((void)at_max.to<int8_t>());
        EXPECT_EQ(127, at_max.to<int8_t>());
    }

    // Test underflow for signed types (negative too small)
    {
        // Create a value representing -129 (too negative for int8_t)
        BFieldElement too_negative = BFieldElement::new_element(BFieldElement::P - 129);
        EXPECT_THROW(too_negative.to<int8_t>(), BFieldElementStringConversionError);

        // Test at exact boundary
        BFieldElement at_min = BFieldElement::new_element(BFieldElement::P - 128);
        EXPECT_NO_THROW((void)at_min.to<int8_t>());
        EXPECT_EQ(-128, at_min.to<int8_t>());
    }

    // Test middle-range values that don't fit in signed types
    {
        // Value in middle of field, doesn't cleanly represent a small signed int
        BFieldElement middle = BFieldElement::new_element(BFieldElement::P / 2);
        EXPECT_THROW((void)middle.to<int32_t>(), BFieldElementStringConversionError);

        // Should still convert to uint64_t though
        EXPECT_NO_THROW((void)middle.to<uint64_t>());
    }

    // Test with various integer sizes
    {
        // Test exact boundaries for uint16_t
        BFieldElement uint16_max = BFieldElement::new_element(65535);
        EXPECT_NO_THROW((void)uint16_max.to<uint16_t>());
        EXPECT_EQ(65535, uint16_max.to<uint16_t>());

        BFieldElement uint16_overflow = BFieldElement::new_element(65536);
        EXPECT_THROW((void)uint16_overflow.to<uint16_t>(), BFieldElementStringConversionError);

        // Test exact boundaries for int16_t
        BFieldElement int16_max = BFieldElement::new_element(32767);
        EXPECT_NO_THROW((void)int16_max.to<int16_t>());
        EXPECT_EQ(32767, int16_max.to<int16_t>());

        BFieldElement int16_min = BFieldElement::new_element(BFieldElement::P - 32768);
        EXPECT_NO_THROW((void)int16_min.to<int16_t>());
        EXPECT_EQ(-32768, int16_min.to<int16_t>());

        BFieldElement int16_overflow = BFieldElement::new_element(32768);
        EXPECT_THROW((void)int16_overflow.to<int16_t>(), BFieldElementStringConversionError);

        BFieldElement int16_underflow = BFieldElement::new_element(BFieldElement::P - 32769);
        EXPECT_THROW((void)int16_underflow.to<int16_t>(), BFieldElementStringConversionError);
    }
}

// Test BFieldElement::mod_reduce function (128-bit to 64-bit reduction)
TEST(BFieldElementTest, ModReduce) {
    // Test simple cases
    EXPECT_EQ(42, BFieldElement::mod_reduce(42));

    // Test boundary values
    EXPECT_EQ(0, BFieldElement::mod_reduce(0));
    EXPECT_EQ(UINT64_MAX, BFieldElement::mod_reduce(UINT64_MAX));

    // Test high bits only (specific to this algorithm)
    EXPECT_EQ(4294967295, BFieldElement::mod_reduce(static_cast<__uint128_t>(1) << 64));
    EXPECT_EQ(UINT64_MAX, BFieldElement::mod_reduce((static_cast<__uint128_t>(1) << 64) - 1));

    // Test specific patterns known to trigger underflow in subtraction
    // This creates a value where x_lo < x_hi_hi, forcing underflow handling
    __uint128_t underflow_case1 = (static_cast<__uint128_t>(0xFFFFFFFF) << 64) | 0x1;
    uint64_t result1 = BFieldElement::mod_reduce(underflow_case1);
    EXPECT_EQ(0xFFFFFFFE00000002ULL, result1);

    // Test another underflow case with specific bit patterns
    __uint128_t underflow_case2 = (static_cast<__uint128_t>(0x00000001) << 96);
    uint64_t result2 = BFieldElement::mod_reduce(underflow_case2);
    EXPECT_EQ(0xFFFFFFFF00000000ULL, result2);

    // Test maximum 128-bit value
    __uint128_t max_128 = ~static_cast<__uint128_t>(0);
    uint64_t result_max = BFieldElement::mod_reduce(max_128);
    EXPECT_EQ(0xFFFFFFFE00000000ULL, result_max);

    // Test random values and verify consistency with the algorithm
    RandomGenerator rng;
    for (int i = 0; i < 100; i++) {
        uint64_t hi = rng.random_range(UINT64_MAX);
        uint64_t lo = rng.random_range(UINT64_MAX);
        __uint128_t val = (static_cast<__uint128_t>(hi) << 64) | lo;

        uint64_t result = BFieldElement::mod_reduce(val);

        // Manual calculation of the algorithm
        uint64_t x_lo = lo;
        uint64_t x_hi_lo = static_cast<uint32_t>(hi);
        uint64_t x_hi_hi = hi >> 32;

        uint64_t tmp0 = x_lo - x_hi_hi;
        bool is_underflow = x_lo < x_hi_hi;

        uint64_t tmp1 = tmp0;
        if (is_underflow) tmp1 -= 0xFFFFFFFF;

        uint64_t tmp2 = (x_hi_lo << 32) - x_hi_lo;

        uint64_t sum = tmp1 + tmp2;
        bool is_overflow = (sum < tmp1) || (sum < tmp2);

        uint64_t expected = sum;
        if (is_overflow) expected += 0xFFFFFFFF;

        EXPECT_EQ(expected, result) << "Failed for hi=" << hi << ", lo=" << lo;
    }
}

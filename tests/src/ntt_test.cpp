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
#include "tip5xx/ntt.hpp"
#include "tip5xx/ntt_error.hpp"
#include "tip5xx/b_field_element.hpp"
#include "tip5xx/x_field_element.hpp"
#include "random_generator.hpp"

using namespace tip5xx;

// Chu NTT B-field property test
TEST(NTTTest, ChuNTTBFieldPropertyTest) {
    RandomGenerator rng;

    for (uint32_t log_2_n = 1; log_2_n < 10; log_2_n++) {
        const size_t n = size_t(1) << log_2_n;
        for (int test = 0; test < 10; test++) {
            std::vector<BFieldElement> values = rng.random_elements(n);
            std::vector<BFieldElement> original_values = values;

            // Test NTT transform
            ntt(values);
            ASSERT_NE(original_values, values);

            // Test inverse transform recovers original values
            intt(values);
            ASSERT_EQ(original_values, values);

            // Test with max element
            values[0] = BFieldElement::new_element(BFieldElement::MAX);
            std::vector<BFieldElement> original_values_with_max = values;

            ntt(values);
            ASSERT_NE(original_values, values);

            intt(values);
            ASSERT_EQ(original_values_with_max, values);
        }
    }
}

// Chu NTT X-field property test
TEST(NTTTest, ChuNTTXFieldPropertyTest) {
    RandomGenerator rng;

    for (uint32_t log_2_n = 1; log_2_n < 10; log_2_n++) {
        const size_t n = size_t(1) << log_2_n;
        for (int test = 0; test < 10; test++) {
            std::vector<XFieldElement> values = rng.random_xfe_elements(n);
            std::vector<XFieldElement> original_values = values;

            // Test NTT transform
            ntt(values);
            ASSERT_NE(original_values, values);

            // Test inverse transform recovers original values
            intt(values);
            ASSERT_EQ(original_values, values);

            // Verify we are not just operating in the B-field
            bool found_non_bfield = false;
            for (const auto& val : original_values) {
                if (!val.coefficients()[1].is_zero() || !val.coefficients()[2].is_zero()) {
                    found_non_bfield = true;
                    break;
                }
            }
            ASSERT_TRUE(found_non_bfield) << "All values appear to be in B-field";

            // Test with max elements
            values[0] = XFieldElement::new_element({
                BFieldElement::MAX,
                BFieldElement::MAX,
                BFieldElement::MAX
            });
            std::vector<XFieldElement> original_values_with_max = values;

            ntt(values);
            ASSERT_NE(original_values, values);

            intt(values);
            ASSERT_EQ(original_values_with_max, values);
        }
    }
}

// X-field basic test of Chu NTT
TEST(NTTTest, XFieldBasicTest) {
    std::vector<XFieldElement> input_output = {
        XFieldElement::new_const(BFieldElement::ONE),
        XFieldElement::ZERO,
        XFieldElement::ZERO,
        XFieldElement::ZERO
    };
    std::vector<XFieldElement> original_input = input_output;
    std::vector<XFieldElement> expected = {
        XFieldElement::new_const(BFieldElement::ONE),
        XFieldElement::new_const(BFieldElement::ONE),
        XFieldElement::new_const(BFieldElement::ONE),
        XFieldElement::new_const(BFieldElement::ONE)
    };

    ntt(input_output);
    ASSERT_EQ(expected, input_output);

    // Verify that INTT(NTT(x)) = x
    intt(input_output);
    ASSERT_EQ(original_input, input_output);
}

// B-field basic test of Chu NTT
TEST(NTTTest, BFieldBasicTest) {
    std::vector<BFieldElement> input_output = {
        BFieldElement::new_element(1),
        BFieldElement::new_element(4),
        BFieldElement::ZERO,
        BFieldElement::ZERO
    };
    std::vector<BFieldElement> original_input = input_output;
    std::vector<BFieldElement> expected = {
        BFieldElement::new_element(5),
        BFieldElement::new_element(1125899906842625ULL),
        BFieldElement::new_element(18446744069414584318ULL),
        BFieldElement::new_element(18445618169507741698ULL)
    };

    ntt(input_output);
    ASSERT_EQ(expected, input_output);

    // Verify that INTT(NTT(x)) = x
    intt(input_output);
    ASSERT_EQ(original_input, input_output);
}

// B-field max value test of Chu NTT
TEST(NTTTest, BFieldMaxValueTest) {
    std::vector<BFieldElement> input_output = {
        BFieldElement::new_element(BFieldElement::MAX),
        BFieldElement::ZERO,
        BFieldElement::ZERO,
        BFieldElement::ZERO
    };
    std::vector<BFieldElement> original_input = input_output;
    std::vector<BFieldElement> expected = {
        BFieldElement::new_element(BFieldElement::MAX),
        BFieldElement::new_element(BFieldElement::MAX),
        BFieldElement::new_element(BFieldElement::MAX),
        BFieldElement::new_element(BFieldElement::MAX)
    };

    ntt(input_output);
    ASSERT_EQ(expected, input_output);

    // Verify that INTT(NTT(x)) = x
    intt(input_output);
    ASSERT_EQ(original_input, input_output);
}

// NTT on empty input
TEST(NTTTest, EmptyInput) {
    std::vector<BFieldElement> input_output;
    std::vector<BFieldElement> original_input = input_output;

    ntt(input_output);
    ASSERT_EQ(0, input_output.size());

    // Verify that INTT(NTT(x)) = x
    intt(input_output);
    ASSERT_EQ(original_input, input_output);
}

// Length one test
TEST(NTTTest, LengthOneTest) {
    RandomGenerator rng;
    BFieldElement bfe = rng.random_bfe();
    std::vector<BFieldElement> test_vector = {bfe};

    ntt(test_vector);
    ASSERT_EQ(std::vector<BFieldElement>{bfe}, test_vector);
}

// NTT then INTT is identity operation
TEST(NTTTest, NTTThenINTTIsIdentityOperation) {
    RandomGenerator rng;

    for (uint32_t i = 0; i < 18; i++) {
        const size_t vector_length = size_t(1) << i;
        std::vector<BFieldElement> input = rng.random_elements(vector_length);
        std::vector<BFieldElement> original_input = input;

        ntt(input);
        intt(input);
        ASSERT_EQ(original_input, input);
    }
}

// B-field NTT with length 32
TEST(NTTTest, BFieldNTTLength32) {
    std::vector<BFieldElement> input_output;
    for (int i = 0; i < 4; i++) {
        input_output.push_back(BFieldElement::new_element(1));
        input_output.push_back(BFieldElement::new_element(4));
        input_output.push_back(BFieldElement::ZERO);
        input_output.push_back(BFieldElement::ZERO);
        input_output.push_back(BFieldElement::ZERO);
        input_output.push_back(BFieldElement::ZERO);
        input_output.push_back(BFieldElement::ZERO);
        input_output.push_back(BFieldElement::ZERO);
    }

    std::vector<BFieldElement> original_input = input_output;

    ntt(input_output);

    std::vector<uint64_t> expected_values = {
        20, 0, 0, 0, 18446744069146148869ULL, 0, 0, 0,
        4503599627370500ULL, 0, 0, 0, 18446726477228544005ULL, 0, 0, 0,
        18446744069414584309ULL, 0, 0, 0, 268435460, 0, 0, 0,
        18442240469787213829ULL, 0, 0, 0, 17592186040324ULL, 0, 0, 0
    };
    std::vector<BFieldElement> expected;
    for (uint64_t val : expected_values) {
        expected.push_back(BFieldElement::new_element(val));
    }

    ASSERT_EQ(expected, input_output);

    // Verify that INTT(NTT(x)) = x
    intt(input_output);
    ASSERT_EQ(original_input, input_output);
}

// NTT noswap test
TEST(NTTTest, NTTNoswapTest) {
    RandomGenerator rng;

    for (uint32_t log_size = 1; log_size < 8; log_size++) {
        const size_t size = size_t(1) << log_size;
        std::vector<BFieldElement> a = rng.random_elements(size);
        std::vector<BFieldElement> a1 = a;
        std::vector<BFieldElement> a2 = a;

        // Compare regular NTT vs noswap
        ntt(a1);
        ntt_noswap(a2);
        bitreverse_order(a2);
        ASSERT_EQ(a1, a2);

        // Compare regular INTT vs noswap
        intt(a1);
        bitreverse_order(a2);
        intt_noswap(a2);
        for (auto& elem : a2) {
            elem *= BFieldElement::new_element(size).inverse();
        }
        ASSERT_EQ(a1, a2);
    }
}

// Test NTT functions with empty vectors
TEST(NTTTest, EmptyVectorTest) {
    // Test with BFieldElement
    {
        // Initialize empty vectors
        std::vector<BFieldElement> empty_bfe;
        std::vector<BFieldElement> empty_bfe_copy;

        // Test ntt
        ASSERT_NO_THROW(ntt(empty_bfe));
        EXPECT_EQ(empty_bfe, empty_bfe_copy);

        // Test intt
        ASSERT_NO_THROW(intt(empty_bfe));
        EXPECT_EQ(empty_bfe, empty_bfe_copy);

        // Test ntt_noswap
        ASSERT_NO_THROW(ntt_noswap(empty_bfe));
        EXPECT_EQ(empty_bfe, empty_bfe_copy);

        // Test intt_noswap
        ASSERT_NO_THROW(intt_noswap(empty_bfe));
        EXPECT_EQ(empty_bfe, empty_bfe_copy);
    }

    // Test with XFieldElement
    {
        // Initialize empty vectors
        std::vector<XFieldElement> empty_xfe;
        std::vector<XFieldElement> empty_xfe_copy;

        // Test ntt
        ASSERT_NO_THROW(ntt(empty_xfe));
        EXPECT_EQ(empty_xfe, empty_xfe_copy);

        // Test intt
        ASSERT_NO_THROW(intt(empty_xfe));
        EXPECT_EQ(empty_xfe, empty_xfe_copy);

        // Test ntt_noswap
        ASSERT_NO_THROW(ntt_noswap(empty_xfe));
        EXPECT_EQ(empty_xfe, empty_xfe_copy);

        // Test intt_noswap
        ASSERT_NO_THROW(intt_noswap(empty_xfe));
        EXPECT_EQ(empty_xfe, empty_xfe_copy);
    }
}

// Test ntt_unchecked with empty vectors (indirect check through ntt)
TEST(NTTTest, UncheckedEmptyVectorTest) {
    std::vector<BFieldElement> empty_bfe;

    // Since ntt() calls ntt_unchecked() internally after size checks,
    // we're implicitly testing that ntt_unchecked handles empty vectors correctly
    // when called through ntt()
    ASSERT_NO_THROW(ntt(empty_bfe));
    EXPECT_EQ(empty_bfe.size(), 0);
}

// Test auxiliary functions with empty vectors
TEST(NTTTest, AuxiliaryEmptyVectorTest) {
    // Test unscale
    {
        std::vector<BFieldElement> empty_bfe;
        ASSERT_NO_THROW(unscale(empty_bfe));
        EXPECT_EQ(empty_bfe.size(), 0);
    }

    // Test bitreverse_order
    {
        std::vector<BFieldElement> empty_bfe;
        std::vector<XFieldElement> empty_xfe;

        ASSERT_NO_THROW(bitreverse_order(empty_bfe));
        EXPECT_EQ(empty_bfe.size(), 0);

        ASSERT_NO_THROW(bitreverse_order(empty_xfe));
        EXPECT_EQ(empty_xfe.size(), 0);
    }
}

// Test NTT functions with non-power-of-2 sizes
TEST(NTTTest, NonPowerOf2Test) {
    // Test with BFieldElement
    {
        // Test various non-power-of-2 vector sizes
        std::vector<size_t> invalid_sizes = {3, 5, 6, 7, 9, 10, 15, 17, 33, 63};

        for (size_t size : invalid_sizes) {
            std::vector<BFieldElement> invalid_bfe(size);
            for (size_t i = 0; i < size; i++) {
                invalid_bfe[i] = BFieldElement::new_element(i);
            }

            // Test ntt
            EXPECT_THROW({
                ntt(invalid_bfe);
            }, NTTInvalidLengthError) << "ntt should throw for non-power-of-2 size: " << size;

            // Test intt
            EXPECT_THROW({
                intt(invalid_bfe);
            }, NTTInvalidLengthError) << "intt should throw for non-power-of-2 size: " << size;

            // Test ntt_noswap
            EXPECT_THROW({
                ntt_noswap(invalid_bfe);
            }, NTTInvalidLengthError) << "ntt_noswap should throw for non-power-of-2 size: " << size;

            // Test intt_noswap
            EXPECT_THROW({
                intt_noswap(invalid_bfe);
            }, NTTInvalidLengthError) << "intt_noswap should throw for non-power-of-2 size: " << size;
        }
    }

    // Test with XFieldElement
    {
        // Test a few non-power-of-2 vector sizes
        std::vector<size_t> invalid_sizes = {3, 6, 15, 33};

        for (size_t size : invalid_sizes) {
            std::vector<XFieldElement> invalid_xfe(size);
            for (size_t i = 0; i < size; i++) {
                invalid_xfe[i] = XFieldElement::new_const(BFieldElement::new_element(i));
            }

            // Test ntt
            EXPECT_THROW({
                ntt(invalid_xfe);
            }, NTTInvalidLengthError) << "ntt should throw for non-power-of-2 size: " << size;

            // Test intt
            EXPECT_THROW({
                intt(invalid_xfe);
            }, NTTInvalidLengthError) << "intt should throw for non-power-of-2 size: " << size;

            // Test ntt_noswap
            EXPECT_THROW({
                ntt_noswap(invalid_xfe);
            }, NTTInvalidLengthError) << "ntt_noswap should throw for non-power-of-2 size: " << size;

            // Test intt_noswap
            EXPECT_THROW({
                intt_noswap(invalid_xfe);
            }, NTTInvalidLengthError) << "intt_noswap should throw for non-power-of-2 size: " << size;
        }
    }
}

// Test exception message
TEST(NTTTest, NonPowerOf2ExceptionMessageTest) {
    std::vector<BFieldElement> invalid_bfe(3);

    try {
        ntt(invalid_bfe);
        FAIL() << "Expected NTTInvalidLengthError exception";
    } catch (const NTTInvalidLengthError& e) {
        EXPECT_STREQ("Input length must be a power of 2", e.what());
    } catch (...) {
        FAIL() << "Expected NTTInvalidLengthError exception";
    }
}

// Test borderline cases (almost powers of 2)
TEST(NTTTest, BorderlineSizeTest) {
    // Test sizes that are near powers of 2
    std::vector<size_t> borderline_sizes = {
        3,      // 4-1
        5,      // 4+1
        7,      // 8-1
        9,      // 8+1
        15,     // 16-1
        17,     // 16+1
        63,     // 64-1
        65,     // 64+1
        127,    // 128-1
        129     // 128+1
    };

    for (size_t size : borderline_sizes) {
        std::vector<BFieldElement> invalid_bfe(size);

        EXPECT_THROW({
            ntt(invalid_bfe);
        }, NTTInvalidLengthError) << "ntt should throw for borderline size: " << size;
    }

    // Valid powers of 2 should not throw
    std::vector<size_t> valid_sizes = {1, 2, 4, 8, 16, 32, 64, 128, 256};

    for (size_t size : valid_sizes) {
        std::vector<BFieldElement> valid_bfe(size);

        EXPECT_NO_THROW(ntt(valid_bfe))
            << "ntt should not throw for valid power-of-2 size: " << size;
    }
}

// Test unscale function
TEST(NTTTest, UnscaleTest) {
    // Test with size 1 vector
    {
        std::vector<BFieldElement> vec = {BFieldElement::new_element(42)};
        std::vector<BFieldElement> expected = {BFieldElement::new_element(42)}; // No change for size 1

        unscale(vec);
        EXPECT_EQ(vec, expected) << "unscale should not change a vector of size 1";
    }

    // Test with various power-of-2 sizes
    for (size_t power = 1; power <= 8; power++) {
        const size_t size = 1ULL << power;
        const BFieldElement size_inverse = BFieldElement::new_element(static_cast<uint64_t>(size)).inverse();

        std::vector<BFieldElement> vec;
        std::vector<BFieldElement> expected;

        for (size_t i = 1; i <= size; i++) {
            BFieldElement value = BFieldElement::new_element(i * 100);
            vec.push_back(value);
            expected.push_back(value * size_inverse);
        }

        unscale(vec);
        EXPECT_EQ(vec, expected)
            << "unscale failed with size " << size;
    }

    // Test with special values
    {
        const size_t size = 8;
        const BFieldElement size_inverse = BFieldElement::new_element(static_cast<uint64_t>(size)).inverse();

        std::vector<BFieldElement> vec = {
            BFieldElement::ZERO,
            BFieldElement::ONE,
            BFieldElement::MAX,
            BFieldElement::new_element(123456789)
        };

        std::vector<BFieldElement> expected = {
            BFieldElement::ZERO,
            size_inverse,
            BFieldElement::MAX * size_inverse,
            BFieldElement::new_element(123456789) * size_inverse
        };

        vec.resize(size, BFieldElement::ZERO); // Pad to size 8
        expected.resize(size, BFieldElement::ZERO);

        unscale(vec);
        EXPECT_EQ(vec, expected) << "unscale failed with special values";
    }

    // Test that unscaling twice is different from unscaling once
    {
        std::vector<BFieldElement> vec = {
            BFieldElement::new_element(10),
            BFieldElement::new_element(20),
            BFieldElement::new_element(30),
            BFieldElement::new_element(40)
        };

        std::vector<BFieldElement> vec_once = vec;
        unscale(vec_once);

        std::vector<BFieldElement> vec_twice = vec;
        unscale(vec_twice);
        unscale(vec_twice);

        EXPECT_NE(vec_once, vec_twice) << "Unscaling twice should be different from unscaling once";
    }

    // Test consistency with INTT implementation
    {
        const size_t size = 8;
        RandomGenerator rng;
        std::vector<BFieldElement> original = rng.random_elements(size);

        // Apply NTT
        std::vector<BFieldElement> ntt_result = original;
        ntt(ntt_result);

        // Create two copies for different INTT approaches
        std::vector<BFieldElement> intt_result = ntt_result;
        std::vector<BFieldElement> manual_intt = ntt_result;

        // Apply built-in INTT
        intt(intt_result);

        // Apply manual INTT (using omega inverse but without scaling)
        const uint32_t log2_size = static_cast<uint32_t>(std::log2(size));
        const BFieldElement omega = BFieldElement::primitive_root_of_unity(size);
        ntt_unchecked(manual_intt, omega.inverse(), log2_size);

        // Now apply unscale separately
        unscale(manual_intt);

        // Both approaches should yield the same result
        EXPECT_EQ(intt_result, manual_intt)
            << "unscale should match the scaling behavior in intt";

        // Both should equal the original input
        EXPECT_EQ(original, intt_result) << "INTT(NTT(x)) should equal x";
    }
}

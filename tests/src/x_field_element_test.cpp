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
#include "tip5xx/x_field_element.hpp"
#include "random_generator.hpp"

using namespace tip5xx;

// Test One and Zero properties
TEST(XFieldElementTest, OneZeroTest) {
    XFieldElement one = XFieldElement::ONE;
    ASSERT_TRUE(one.is_one());
    ASSERT_TRUE(one.coefficients()[0].is_one());
    ASSERT_TRUE(one.coefficients()[1].is_zero());
    ASSERT_TRUE(one.coefficients()[2].is_zero());
    ASSERT_EQ(one, XFieldElement::ONE);

    XFieldElement zero = XFieldElement::ZERO;
    ASSERT_TRUE(zero.is_zero());
    ASSERT_TRUE(zero.coefficients()[0].is_zero());
    ASSERT_TRUE(zero.coefficients()[1].is_zero());
    ASSERT_TRUE(zero.coefficients()[2].is_zero());
    ASSERT_EQ(zero, XFieldElement::ZERO);

    XFieldElement two = XFieldElement::new_const(BFieldElement::new_element(2));
    ASSERT_FALSE(two.is_one());
    ASSERT_FALSE(zero.is_one());

    XFieldElement one_with_x = XFieldElement::new_element({
        BFieldElement::ONE,
        BFieldElement::ONE,
        BFieldElement::ZERO
    });
    XFieldElement one_with_x2 = XFieldElement::new_element({
        BFieldElement::ONE,
        BFieldElement::ZERO,
        BFieldElement::ONE
    });
    ASSERT_FALSE(one_with_x.is_one());
    ASSERT_FALSE(one_with_x2.is_one());
    ASSERT_FALSE(one_with_x.is_zero());
    ASSERT_FALSE(one_with_x2.is_zero());
}

// Test random element generation
TEST(XFieldElementTest, RandomElementGenerationTest) {
    RandomGenerator rng;
    std::vector<XFieldElement> rand_xs = rng.random_xfe_elements(14);
    ASSERT_EQ(14, rand_xs.size());

    // Verify uniqueness
    std::unordered_set<std::string> unique_elements;
    for (const auto& x : rand_xs) {
        std::string element_str = x.to_string();
        ASSERT_TRUE(unique_elements.insert(element_str).second)
            << "Duplicate element found: " << element_str;
    }
}

// Test increment/decrement functionality
TEST(XFieldElementTest, IncrDecrTest) {
    XFieldElement val = XFieldElement::ZERO;

    // Test coefficient 0
    val.increment(0);
    ASSERT_TRUE(val.is_one());
    val.increment(0);
    ASSERT_EQ(val, XFieldElement::new_const(BFieldElement::new_element(2)));
    val.decrement(0);
    ASSERT_TRUE(val.is_one());
    val.decrement(0);
    ASSERT_TRUE(val.is_zero());

    // Test coefficient 1 (x term)
    val.increment(1);
    ASSERT_EQ(val, XFieldElement::new_element({
        BFieldElement::ZERO,
        BFieldElement::ONE,
        BFieldElement::ZERO
    }));
    val.decrement(1);
    ASSERT_TRUE(val.is_zero());

    // Test coefficient 2 (x^2 term)
    val.increment(2);
    ASSERT_EQ(val, XFieldElement::new_element({
        BFieldElement::ZERO,
        BFieldElement::ZERO,
        BFieldElement::ONE
    }));
    val.decrement(2);
    ASSERT_TRUE(val.is_zero());
}

// Test addition
TEST(XFieldElementTest, AdditionTest) {
    // Test constant terms
    {
        XFieldElement a = XFieldElement::new_const(BFieldElement::new_element(2));
        XFieldElement b = XFieldElement::new_const(BFieldElement::new_element(3));
        XFieldElement sum = XFieldElement::new_const(BFieldElement::new_element(5));
        ASSERT_EQ(sum, a + b);
    }

    // Test x terms
    {
        XFieldElement a = XFieldElement::new_element({
            BFieldElement::ZERO,
            BFieldElement::new_element(5),
            BFieldElement::ZERO
        });
        XFieldElement b = XFieldElement::new_element({
            BFieldElement::ZERO,
            BFieldElement::new_element(7),
            BFieldElement::ZERO
        });
        XFieldElement sum = XFieldElement::new_element({
            BFieldElement::ZERO,
            BFieldElement::new_element(12),
            BFieldElement::ZERO
        });
        ASSERT_EQ(sum, a + b);
    }

    // Test x^2 terms
    {
        XFieldElement a = XFieldElement::new_element({
            BFieldElement::ZERO,
            BFieldElement::ZERO,
            BFieldElement::new_element(14)
        });
        XFieldElement b = XFieldElement::new_element({
            BFieldElement::ZERO,
            BFieldElement::ZERO,
            BFieldElement::new_element(23)
        });
        XFieldElement sum = XFieldElement::new_element({
            BFieldElement::ZERO,
            BFieldElement::ZERO,
            BFieldElement::new_element(37)
        });
        ASSERT_EQ(sum, a + b);
    }

    // Test mixed terms
    {
        XFieldElement a = XFieldElement::new_element({
            BFieldElement::MAX - BFieldElement::new_element(2),
            BFieldElement::new_element(12),
            BFieldElement::new_element(4)
        });
        XFieldElement b = XFieldElement::new_element({
            BFieldElement::new_element(2),
            BFieldElement::new_element(45000),
            BFieldElement::MAX - BFieldElement::new_element(3)
        });
        XFieldElement sum = XFieldElement::new_element({
            BFieldElement::MAX,
            BFieldElement::new_element(45012),
            BFieldElement::new_element(0)
        });
        ASSERT_EQ(sum, a + b);
    }
}

// Test multiplication
TEST(XFieldElementTest, MultiplicationTest) {
    // Test constants
    {
        XFieldElement a = XFieldElement::new_const(BFieldElement::new_element(2));
        XFieldElement b = XFieldElement::new_const(BFieldElement::new_element(3));
        XFieldElement prod = XFieldElement::new_const(BFieldElement::new_element(6));
        ASSERT_EQ(prod, a * b);
    }

    // Test x terms
    {
        XFieldElement a = XFieldElement::new_element({
            BFieldElement::ZERO,
            BFieldElement::new_element(3),
            BFieldElement::ZERO
        });
        XFieldElement b = XFieldElement::new_element({
            BFieldElement::ZERO,
            BFieldElement::new_element(3),
            BFieldElement::ZERO
        });
        XFieldElement prod = XFieldElement::new_element({
            BFieldElement::ZERO,
            BFieldElement::ZERO,
            BFieldElement::new_element(9)
        });
        ASSERT_EQ(prod, a * b);
    }

    // Test mixed polynomial multiplication
    {
        // (13+2x+3x^2)(19+5x^2) = 247+38x+122x^2 (mod x^3-x+1)
        XFieldElement a = XFieldElement::new_element({
            BFieldElement::new_element(13),
            BFieldElement::new_element(2),
            BFieldElement::new_element(3)
        });
        XFieldElement b = XFieldElement::new_element({
            BFieldElement::new_element(19),
            BFieldElement::ZERO,
            BFieldElement::new_element(5)
        });
        XFieldElement prod = XFieldElement::new_element({
            BFieldElement::new_element(237),
            BFieldElement::new_element(33),
            BFieldElement::new_element(137)
        });
        ASSERT_EQ(prod, a * b);
    }

    // Test x^2 * x^2 = x^4 = x^2 - x (mod x^3-x+1)
    {
        XFieldElement x2 = XFieldElement::new_element({
            BFieldElement::ZERO,
            BFieldElement::ZERO,
            BFieldElement::ONE
        });
        XFieldElement result = XFieldElement::new_element({
            BFieldElement::ZERO,
            BFieldElement::MAX,
            BFieldElement::ONE
        });
        ASSERT_EQ(result, x2 * x2);
    }
}

// Test inversion
/*TEST(XFieldElementTest, InversionTest) {
    RandomGenerator rng;

    for (int i = 0; i < 100; i++) {
        XFieldElement x = rng.random_xfe();
        if (x.is_zero()) continue;

        XFieldElement x_inv = x.inverse();
        XFieldElement product = x * x_inv;

        ASSERT_TRUE(product.is_one())
            << "Inverse failed for x = " << x.to_string()
            << ", x_inv = " << x_inv.to_string();
    }

    // Test specific values
    XFieldElement one = XFieldElement::ONE;
    ASSERT_EQ(one, one.inverse());

    XFieldElement two = XFieldElement::new_const(BFieldElement::new_element(2));
    XFieldElement two_inv = two.inverse();
    ASSERT_TRUE((two * two_inv).is_one());

    // Test that inverting zero throws
    XFieldElement zero = XFieldElement::ZERO;
    EXPECT_THROW(zero.inverse(), XFieldElementInverseError);
} */

// Test subtraction
TEST(XFieldElementTest, SubtractionTest) {
    XFieldElement two = XFieldElement::new_const(BFieldElement::new_element(2));
    XFieldElement three = XFieldElement::new_const(BFieldElement::new_element(3));
    XFieldElement one = XFieldElement::ONE;

    ASSERT_EQ(one, three - two);

    // Test with x terms
    XFieldElement x5 = XFieldElement::new_element({
        BFieldElement::ZERO,
        BFieldElement::new_element(5),
        BFieldElement::ZERO
    });
    XFieldElement x7 = XFieldElement::new_element({
        BFieldElement::ZERO,
        BFieldElement::new_element(7),
        BFieldElement::ZERO
    });
    XFieldElement x2 = XFieldElement::new_element({
        BFieldElement::ZERO,
        BFieldElement::new_element(2),
        BFieldElement::ZERO
    });

    ASSERT_EQ(x2, x7 - x5);
}

// Test field properties
TEST(XFieldElementTest, FieldProperties) {
    RandomGenerator rng;
    std::vector<XFieldElement> elements;
    for (int i = 0; i < 5; i++) {
        elements.push_back(rng.random_xfe());
    }

    // Test associativity of addition
    for (int i = 0; i < elements.size() - 2; i++) {
        XFieldElement a = elements[i];
        XFieldElement b = elements[i + 1];
        XFieldElement c = elements[i + 2];

        ASSERT_EQ((a + b) + c, a + (b + c))
            << "Addition is not associative";
    }

    // Test associativity of multiplication
    for (int i = 0; i < elements.size() - 2; i++) {
        XFieldElement a = elements[i];
        XFieldElement b = elements[i + 1];
        XFieldElement c = elements[i + 2];

        ASSERT_EQ((a * b) * c, a * (b * c))
            << "Multiplication is not associative";
    }

    // Test distributive property
    for (int i = 0; i < elements.size() - 2; i++) {
        XFieldElement a = elements[i];
        XFieldElement b = elements[i + 1];
        XFieldElement c = elements[i + 2];

        ASSERT_EQ(a * (b + c), a * b + a * c)
            << "Multiplication is not distributive over addition";
    }
}

// Test B-field operations
TEST(XFieldElementTest, BFieldOperations) {
    RandomGenerator rng;

    for (int i = 0; i < 100; i++) {
        XFieldElement xfe = rng.random_xfe();
        BFieldElement bfe = rng.random_bfe();

        // Test XFE + BFE = BFE + XFE
        XFieldElement sum1 = xfe + bfe;
        XFieldElement sum2 = bfe + xfe;
        ASSERT_EQ(sum1, sum2) << "Addition with BFieldElement is not commutative";

        // Test XFE * BFE = BFE * XFE
        XFieldElement prod1 = xfe * bfe;
        XFieldElement prod2 = bfe * xfe;
        ASSERT_EQ(prod1, prod2) << "Multiplication with BFieldElement is not commutative";

        // Test XFE - BFE
        XFieldElement diff = xfe - bfe;
        ASSERT_EQ(xfe, diff + bfe) << "Subtraction with BFieldElement failed";
    }
}

// Test batch inversion
/*TEST(XFieldElementTest, BatchInversion) {
    // Test empty batch
    std::vector<XFieldElement> empty_inv = XFieldElement::batch_inversion({});
    ASSERT_TRUE(empty_inv.empty());

    // Test single element
    std::vector<XFieldElement> single = {XFieldElement::ONE};
    std::vector<XFieldElement> single_inv = XFieldElement::batch_inversion(single);
    ASSERT_EQ(1, single_inv.size());
    ASSERT_TRUE(single_inv[0].is_one());

    // Test multiple elements
    RandomGenerator rng;
    std::vector<XFieldElement> elements;
    for (int i = 0; i < 10; i++) {
        XFieldElement elem = rng.random_xfe();
        if (!elem.is_zero()) {  // Skip zero elements
            elements.push_back(elem);
        }
    }

    std::vector<XFieldElement> inverses = XFieldElement::batch_inversion(elements);
    ASSERT_EQ(elements.size(), inverses.size());

    // Verify each inverse
    for (size_t i = 0; i < elements.size(); i++) {
        ASSERT_TRUE((elements[i] * inverses[i]).is_one())
            << "Batch inversion failed for element " << i;
    }
}
*/
// Copyright (c) 2025 Maxim [maxirmx] Samsonov (maxirmx@gmail.com)
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

#include <gtest/gtest.h>
#include <tip5xx/polynomial.hpp>
#include <tip5xx/b_field_element.hpp>
#include <tip5xx/x_field_element.hpp>
#include <optional>

using namespace tip5xx;
using namespace std;

TEST(PolynomialTest, Display) {
    // Test zero polynomial display
    EXPECT_EQ("0", Polynomial<BFieldElement>().to_string());

    // Test constant term only
    vector<BFieldElement> const_coeffs = {bfe_from(42)};
    EXPECT_EQ("42", Polynomial<BFieldElement>(const_coeffs).to_string());

    // Test single variable x
    vector<BFieldElement> x_coeffs = {BFieldElement::zero(), BFieldElement::one()};
    EXPECT_EQ("x", Polynomial<BFieldElement>(x_coeffs).to_string());

    // Test 2x display
    vector<BFieldElement> two_x_coeffs = {BFieldElement::zero(), bfe_from(2)};
    EXPECT_EQ("2x", Polynomial<BFieldElement>(two_x_coeffs).to_string());

    // Test linear term with constant: 5x + 2
    vector<BFieldElement> linear_coeffs = {bfe_from(2), bfe_from(5)};
    EXPECT_EQ("5x + 2", Polynomial<BFieldElement>(linear_coeffs).to_string());

    // Test higher degree terms: 4x^4 + 3x^3 + 0x^2 + 0x + 0
    vector<BFieldElement> higher_coeffs = {
        BFieldElement::zero(),
        BFieldElement::zero(),
        BFieldElement::zero(),
        bfe_from(3),
        bfe_from(4)
    };
    EXPECT_EQ("4x^4 + 3x^3", Polynomial<BFieldElement>(higher_coeffs).to_string());
}

TEST(PolynomialTest, LeadingCoefficientOfZeroPolynomialIsNone) {
    for (size_t num_zeros = 0; num_zeros < 30; ++num_zeros) {
        vector<BFieldElement> coefficients(num_zeros, BFieldElement::zero());
        Polynomial<BFieldElement> polynomial(coefficients);
        EXPECT_FALSE(polynomial.leading_coefficient().has_value());
    }
}

TEST(PolynomialTest, LeadingCoefficientOfNonZeroPolynomial) {
    for (size_t num_leading_zeros = 0; num_leading_zeros < 30; ++num_leading_zeros) {
        // Base polynomial with some coefficients
        vector<BFieldElement> base_coeffs = {bfe_from(1), bfe_from(2)};
        BFieldElement leading_coeff = bfe_from(42);

        // Add leading coefficient and trailing zeros
        vector<BFieldElement> coefficients = base_coeffs;
        coefficients.push_back(leading_coeff);
        coefficients.insert(coefficients.end(), num_leading_zeros, BFieldElement::zero());

        Polynomial<BFieldElement> polynomial(coefficients);
        EXPECT_TRUE(polynomial.leading_coefficient().has_value());
        EXPECT_EQ(leading_coeff, polynomial.leading_coefficient().value());
    }
}

TEST(PolynomialTest, NormalizingCanonicalZeroPolynomialHasNoEffect) {
    Polynomial<BFieldElement> zero_polynomial;
    zero_polynomial.normalize();
    EXPECT_EQ(Polynomial<BFieldElement>(), zero_polynomial);
}

TEST(PolynomialTest, SpuriousLeadingZerosDontAffectEquality) {
    vector<BFieldElement> base_coeffs = {bfe_from(1), bfe_from(2)};
    Polynomial<BFieldElement> base_poly(base_coeffs);

    for (size_t num_leading_zeros = 0; num_leading_zeros < 30; ++num_leading_zeros) {
        vector<BFieldElement> extended_coeffs = base_coeffs;
        extended_coeffs.insert(extended_coeffs.end(), num_leading_zeros, BFieldElement::zero());
        Polynomial<BFieldElement> extended_poly(extended_coeffs);

        EXPECT_EQ(base_poly, extended_poly);
    }
}

TEST(PolynomialTest, NormalizingRemovesSpuriousLeadingZeros) {
    vector<BFieldElement> base_coeffs = {bfe_from(1), bfe_from(2)};
    BFieldElement leading_coeff = bfe_from(42);

    for (size_t num_leading_zeros = 0; num_leading_zeros < 30; ++num_leading_zeros) {
        vector<BFieldElement> coeffs = base_coeffs;
        coeffs.push_back(leading_coeff);
        coeffs.insert(coeffs.end(), num_leading_zeros, BFieldElement::zero());

        Polynomial<BFieldElement> poly(coeffs);
        poly.normalize();

        size_t expected_size = base_coeffs.size() + 1; // +1 for leading_coeff
        EXPECT_EQ(expected_size, poly.coefficients().size());
    }
}

TEST(PolynomialTest, AccessingCoefficientsOfEmptyPolynomialGivesEmptyVector) {
    Polynomial<BFieldElement> poly;
    EXPECT_TRUE(poly.coefficients().empty());
}

TEST(PolynomialTest, AccessingCoefficientsOfPolynomialWithOnlyZeroCoefficients) {
    for (size_t num_zeros = 0; num_zeros < 30; ++num_zeros) {
        vector<BFieldElement> coeffs(num_zeros, BFieldElement::zero());
        Polynomial<BFieldElement> poly(coeffs);
        Polynomial<BFieldElement> normalized_poly(coeffs);
        normalized_poly.normalize();
        EXPECT_TRUE(normalized_poly.coefficients().empty());
    }
}

TEST(PolynomialTest, AccessingCoefficientsIsEquivalentToNormalizingThenRawAccess) {
    for (size_t num_leading_zeros = 0; num_leading_zeros < 30; ++num_leading_zeros) {
        vector<BFieldElement> coeffs = {bfe_from(1), bfe_from(2), bfe_from(3)};
        coeffs.insert(coeffs.end(), num_leading_zeros, BFieldElement::zero());

        Polynomial<BFieldElement> poly(coeffs);
        Polynomial<BFieldElement> poly1(coeffs);
        auto accessed_coeffs = poly1.coefficients();

        Polynomial<BFieldElement> poly2(coeffs);
        poly2.normalize();
        auto normalized_coeffs = poly2.coefficients();

        EXPECT_EQ(normalized_coeffs, accessed_coeffs);
    }
}

TEST(PolynomialTest, XToThe0IsConstant1) {
    EXPECT_TRUE(Polynomial<BFieldElement>::x_to_the(0).is_one());
    EXPECT_TRUE(Polynomial<XFieldElement>::x_to_the(0).is_one());
}

TEST(PolynomialTest, XToThe1IsX) {
    EXPECT_TRUE(Polynomial<BFieldElement>::x_to_the(1).is_x());
    EXPECT_TRUE(Polynomial<XFieldElement>::x_to_the(1).is_x());
}

TEST(PolynomialTest, XToTheNToTheMIsHomomorphic) {
    for (size_t n = 0; n < 5; ++n) {
        for (size_t m = 0; m < 5; ++m) {
            auto to_the_n_times_m = Polynomial<BFieldElement>::x_to_the(n * m);
            auto to_the_n_then_to_the_m = Polynomial<BFieldElement>::x_to_the(n).pow(m);
            EXPECT_EQ(to_the_n_times_m, to_the_n_then_to_the_m);
        }
    }
}

TEST(PolynomialTest, ScalingPolynomialWorks) {
    vector<BFieldElement> coeffs = {BFieldElement::zero(), bfe_from(1), bfe_from(2)};
    Polynomial<BFieldElement> bfe_poly(coeffs);

    auto scaled_by_bfe = bfe_poly.scale(bfe_from(42));
    auto scaled_by_xfe = bfe_poly.scale(xfe_from(42));

    EXPECT_NE(bfe_poly, scaled_by_bfe);
    EXPECT_NE(bfe_poly, scaled_by_xfe);
}

TEST(PolynomialTest, ScalingEquivalentInExtensionField) {
    vector<BFieldElement> coeffs = {bfe_from(1), bfe_from(2), bfe_from(3)};
    Polynomial<BFieldElement> bfe_poly(coeffs);

    BFieldElement alpha = bfe_from(42);

    // Convert BFieldElement polynomial to XFieldElement polynomial
    vector<XFieldElement> xfe_coeffs;
    for (const auto& coeff : coeffs) {
        xfe_coeffs.push_back(coeff.lift());
    }
    Polynomial<XFieldElement> xfe_poly(xfe_coeffs);

    auto xfe_poly_bfe_scalar = xfe_poly.scale(alpha);
    auto bfe_poly_xfe_scalar = bfe_poly.scale(alpha);

    EXPECT_EQ(xfe_poly_bfe_scalar, bfe_poly_xfe_scalar);
}
/*
TEST(PolynomialTest, ScalarMultiplicationMethodsEquivalent) {
    vector<BFieldElement> coeffs = {bfe_from(1), bfe_from(2), bfe_from(3)};
    Polynomial<BFieldElement> poly(coeffs);
    BFieldElement scalar = bfe_from(42);

    auto scalar_mul_result = poly * scalar;

    auto mut_poly = poly;
    mut_poly.scalar_mul_mut(scalar);

    EXPECT_EQ(mut_poly, scalar_mul_result);
}
*/
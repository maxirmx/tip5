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

#include <cassert>
#include <sstream>
#include "tip5xx/x_field_element.hpp"
#include "tip5xx/x_field_element_error.hpp"

namespace tip5xx {

const XFieldElement XFieldElement::ZERO = XFieldElement({BFieldElement::ZERO, BFieldElement::ZERO, BFieldElement::ZERO});
const XFieldElement XFieldElement::ONE = XFieldElement({BFieldElement::ONE, BFieldElement::ZERO, BFieldElement::ZERO});

XFieldElement XFieldElement::inverse_impl() const {
    if (is_zero()) {
        throw XFieldElementInverseError();
    }

    // Calculate inverse using a*a^(-1) ≡ 1 (mod x³ - x + 1)
    // Given that our field is defined by x³ - x + 1
    const BFieldElement& a = coefficients_[0];
    const BFieldElement& b = coefficients_[1];
    const BFieldElement& c = coefficients_[2];

    // Compute denominator: (a² - bc) - (ac - b²)x + c²x²
    BFieldElement a_sqr = a.square();
    BFieldElement b_sqr = b.square();
    BFieldElement c_sqr = c.square();
    BFieldElement bc = b * c;
    BFieldElement ac = a * c;

    BFieldElement denom_0 = a_sqr - bc;
    BFieldElement denom_1 = b_sqr - ac;
    BFieldElement denom_2 = c_sqr;

    // inv = ((a² - bc) + (b² - ac)x + c²x²) / ((a² - bc)² + (b² - ac)² + c⁴)
    BFieldElement inv_denom = denom_0.square() + denom_1.square() + denom_2.square();
    inv_denom = inv_denom.inverse();

    return XFieldElement({
        denom_0 * inv_denom,
        denom_1 * inv_denom,
        denom_2 * inv_denom
    });
}

XFieldElement XFieldElement::primitive_root_of_unity_impl(uint64_t n) {
    // For extension field, we can use the base field's primitive root lifted
    try {
        auto b_root = BFieldElement::primitive_root_of_unity(n);
        return new_const(b_root);
    } catch (const std::runtime_error& e) {
        throw XFieldElementError(XFieldElementError::ErrorType::NoRootOfUnity,
            "No primitive root of unity exists for the given order");
    }
}

XFieldElement XFieldElement::mod_pow_u64_impl(uint64_t exp) const {
    XFieldElement result = ONE;
    XFieldElement base = *this;

    while (exp > 0) {
        if (exp & 1) {
            result *= base;
        }
        base *= base;
        exp >>= 1;
    }

    return result;
}

XFieldElement XFieldElement::mod_pow_u32_impl(uint32_t exp) const {
    return mod_pow_u64_impl(static_cast<uint64_t>(exp));
}

std::vector<XFieldElement> XFieldElement::cyclic_group_elements_impl(size_t max) const {
    std::vector<XFieldElement> result;
    result.push_back(ONE);

    XFieldElement current = *this;
    while (!current.is_one() && (max == 0 || result.size() < max)) {
        result.push_back(current);
        current *= *this;
    }

    return result;
}

BFieldElement XFieldElement::unlift() const {
    if (coefficients_[1].is_zero() && coefficients_[2].is_zero()) {
        return coefficients_[0];
    }
    throw XFieldElementError(XFieldElementError::ErrorType::InvalidUnlift,
        "Cannot unlift: element has non-zero coefficients for x or x²");
}

void XFieldElement::increment(size_t index) {
    assert(index < EXTENSION_DEGREE);
    coefficients_[index].increment();
}

void XFieldElement::decrement(size_t index) {
    assert(index < EXTENSION_DEGREE);
    coefficients_[index].decrement();
}

XFieldElement XFieldElement::operator+(const XFieldElement& rhs) const {
    return XFieldElement({
        coefficients_[0] + rhs.coefficients_[0],
        coefficients_[1] + rhs.coefficients_[1],
        coefficients_[2] + rhs.coefficients_[2]
    });
}

XFieldElement& XFieldElement::operator+=(const XFieldElement& rhs) {
    coefficients_[0] += rhs.coefficients_[0];
    coefficients_[1] += rhs.coefficients_[1];
    coefficients_[2] += rhs.coefficients_[2];
    return *this;
}

XFieldElement XFieldElement::operator+(const BFieldElement& rhs) const {
    return XFieldElement({
        coefficients_[0] + rhs,
        coefficients_[1],
        coefficients_[2]
    });
}

XFieldElement& XFieldElement::operator+=(const BFieldElement& rhs) {
    coefficients_[0] += rhs;
    return *this;
}

XFieldElement XFieldElement::operator-(const XFieldElement& rhs) const {
    return XFieldElement({
        coefficients_[0] - rhs.coefficients_[0],
        coefficients_[1] - rhs.coefficients_[1],
        coefficients_[2] - rhs.coefficients_[2]
    });
}

XFieldElement& XFieldElement::operator-=(const XFieldElement& rhs) {
    coefficients_[0] -= rhs.coefficients_[0];
    coefficients_[1] -= rhs.coefficients_[1];
    coefficients_[2] -= rhs.coefficients_[2];
    return *this;
}

XFieldElement XFieldElement::operator-(const BFieldElement& rhs) const {
    return XFieldElement({
        coefficients_[0] - rhs,
        coefficients_[1],
        coefficients_[2]
    });
}

XFieldElement& XFieldElement::operator-=(const BFieldElement& rhs) {
    coefficients_[0] -= rhs;
    return *this;
}

XFieldElement XFieldElement::operator*(const XFieldElement& rhs) const {
    // Multiplication in the extension field modulo x³ - x + 1
    // (ax² + bx + c)(dx² + ex + f) =
    // adx⁴ + (ae + bd)x³ + (af + be + cd)x² + (bf + ce)x + cf
    // Reduce using x³ = x - 1

    const BFieldElement& a = coefficients_[2];
    const BFieldElement& b = coefficients_[1];
    const BFieldElement& c = coefficients_[0];
    const BFieldElement& d = rhs.coefficients_[2];
    const BFieldElement& e = rhs.coefficients_[1];
    const BFieldElement& f = rhs.coefficients_[0];

    // r0 = cf - ae - bd  (from reducing x^4 and x^3 terms)
    // r1 = bf + ce - ad + ae + bd  (from collecting x terms)
    // r2 = af + be + cd + ad  (from collecting x^2 terms)
    BFieldElement r0 = c * f - a * e - b * d;
    BFieldElement r1 = b * f + c * e - a * d + a * e + b * d;
    BFieldElement r2 = a * f + b * e + c * d + a * d;

    return XFieldElement({r0, r1, r2});
}

XFieldElement& XFieldElement::operator*=(const XFieldElement& rhs) {
    *this = *this * rhs;
    return *this;
}

XFieldElement XFieldElement::operator*(const BFieldElement& rhs) const {
    return XFieldElement({
        coefficients_[0] * rhs,
        coefficients_[1] * rhs,
        coefficients_[2] * rhs
    });
}

XFieldElement& XFieldElement::operator*=(const BFieldElement& rhs) {
    coefficients_[0] *= rhs;
    coefficients_[1] *= rhs;
    coefficients_[2] *= rhs;
    return *this;
}

XFieldElement XFieldElement::operator/(const XFieldElement& rhs) const {
    return *this * rhs.inverse();
}

XFieldElement XFieldElement::operator-() const {
    return XFieldElement({
        -coefficients_[0],
        -coefficients_[1],
        -coefficients_[2]
    });
}

std::string XFieldElement::to_string() const {
    // Try to unlift and show as base field element
    try {
        BFieldElement base = unlift();
        std::stringstream ss;
        ss << base << "_xfe";
        return ss.str();
    } catch (const XFieldElementError&) {
        std::stringstream ss;
        ss << "(" << coefficients_[2] << "·x² + "
           << coefficients_[1] << "·x + "
           << coefficients_[0] << ")";
        return ss.str();
    }
}

std::ostream& operator<<(std::ostream& os, const XFieldElement& xfe) {
    return os << xfe.to_string();
}

std::istream& operator>>(std::istream& is, XFieldElement& xfe) {
    std::array<BFieldElement, XFieldElement::EXTENSION_DEGREE> coeffs;
    char c;
    is >> coeffs[0] >> c >> coeffs[1] >> c >> coeffs[2];
    xfe = XFieldElement::new_element(coeffs);
    return is;
}

} // namespace tip5xx

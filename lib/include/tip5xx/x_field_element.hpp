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

#pragma once

#include <array>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "b_field_element.hpp"
#include "traits.hpp"
#include "x_field_element_error.hpp"

namespace tip5xx {

/**
 * Extension field element (cube extension) over base field.
 * Represents elements of the field ℤ_{2^64 - 2^32 + 1}[x]/(x³ - x + 1)
 */
class XFieldElement : public FiniteField<XFieldElement> {
public:
    using FiniteField<XFieldElement>::inverse;
    using FiniteField<XFieldElement>::inverse_or_zero;
    using FiniteField<XFieldElement>::batch_inversion;
    using FiniteField<XFieldElement>::cyclic_group_elements;
    using FiniteField<XFieldElement>::primitive_root_of_unity;
    using FiniteField<XFieldElement>::mod_pow_u64;
    using FiniteField<XFieldElement>::mod_pow_u32;
    using FiniteField<XFieldElement>::square;

    static constexpr size_t EXTENSION_DEGREE = 3;

    // Constants
    static const XFieldElement ZERO;
    static const XFieldElement ONE;

    // Constructors
    XFieldElement() : coefficients_{{BFieldElement::ZERO, BFieldElement::ZERO, BFieldElement::ZERO}} {}

    // Main constructor
    static XFieldElement new_element(const std::array<BFieldElement, EXTENSION_DEGREE>& coeffs) {
        return XFieldElement(coeffs);
    }

    // Create constant element from a single coefficient
    static XFieldElement new_const(const BFieldElement& element) {
        return XFieldElement({element, BFieldElement::ZERO, BFieldElement::ZERO});
    }

    // Get coefficients
    const std::array<BFieldElement, EXTENSION_DEGREE>& coefficients() const {
        return coefficients_;
    }

    // Implementation of Inverse trait
    XFieldElement inverse_impl() const;

    // Implementation of PrimitiveRootOfUnity trait
    static XFieldElement primitive_root_of_unity_impl(uint64_t n);

    // Implementation of ModPowU64 trait
    XFieldElement mod_pow_u64_impl(uint64_t exp) const;

    // Implementation of ModPowU32 trait
    XFieldElement mod_pow_u32_impl(uint32_t exp) const;

    // Implementation of CyclicGroupGenerator trait
    std::vector<XFieldElement> cyclic_group_elements_impl(size_t max = 0) const;

    // Static methods required by FiniteField
    static XFieldElement zero() { return ZERO; }
    static XFieldElement one() { return ONE; }

    // Convert back to base field element
    // Throws XFieldElementError if element cannot be unlifted
    BFieldElement unlift() const;

    // Testing utilities
    void increment(size_t index);
    void decrement(size_t index);

    // Check if zero/one
    bool is_zero() const {
        return *this == ZERO;
    }

    bool is_one() const {
        return *this == ONE;
    }

    // Arithmetic operators
    XFieldElement operator+(const XFieldElement& rhs) const;
    XFieldElement& operator+=(const XFieldElement& rhs);

    XFieldElement operator+(const BFieldElement& rhs) const;
    XFieldElement& operator+=(const BFieldElement& rhs);

    friend XFieldElement operator+(const BFieldElement& lhs, const XFieldElement& rhs) {
        return rhs + lhs;
    }

    XFieldElement operator-(const XFieldElement& rhs) const;
    XFieldElement& operator-=(const XFieldElement& rhs);

    XFieldElement operator-(const BFieldElement& rhs) const;
    XFieldElement& operator-=(const BFieldElement& rhs);

    friend XFieldElement operator-(const BFieldElement& lhs, const XFieldElement& rhs) {
        return -rhs + lhs;
    }

    XFieldElement operator*(const XFieldElement& rhs) const;
    XFieldElement& operator*=(const XFieldElement& rhs);

    XFieldElement operator*(const BFieldElement& rhs) const;
    XFieldElement& operator*=(const BFieldElement& rhs);

    friend XFieldElement operator*(const BFieldElement& lhs, const XFieldElement& rhs) {
        return rhs * lhs;
    }

    XFieldElement operator/(const XFieldElement& rhs) const;

    XFieldElement operator-() const;

    // Equality operators
    bool operator==(const XFieldElement& rhs) const {
        return coefficients_ == rhs.coefficients_;
    }

    bool operator!=(const XFieldElement& rhs) const {
        return !(*this == rhs);
    }

    // String conversion
    std::string to_string() const;

private:
    std::array<BFieldElement, EXTENSION_DEGREE> coefficients_;

    // Private constructor from raw coefficients
    explicit XFieldElement(const std::array<BFieldElement, EXTENSION_DEGREE>& coeffs)
        : coefficients_(coeffs) {}
};

// Stream operators
std::ostream& operator<<(std::ostream& os, const XFieldElement& xfe);
std::istream& operator>>(std::istream& is, XFieldElement& xfe);

// Macros for convenient creation
#define xfe(x) XFieldElement::new_const(BFieldElement::new_element(x))
#define xfe_vec(...) createXfeVec(__VA_ARGS__)
#define xfe_array(...) createXfeArray(__VA_ARGS__)

// Helper functions for macros
template<typename... Args>
std::vector<XFieldElement> createXfeVec(Args... args);

template<size_t N, typename... Args>
std::array<XFieldElement, N> createXfeArray(Args... args);

} // namespace tip5xx

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

#pragma once

#include "tip5xx/b_field_element.hpp"
#include "tip5xx/x_field_element.hpp"
#include "tip5xx/ntt.hpp"
#include <vector>
#include <optional>
#include <algorithm>
#include <numeric>

namespace tip5xx {

/**
 * @brief A univariate polynomial with coefficients in a finite field, in monomial form.
 *
 * @tparam FF Finite field type for coefficients
 */
template<typename FF>
class Polynomial {
public:
    // Constructors
    Polynomial();  // Zero polynomial
    explicit Polynomial(const std::vector<FF>& coefficients);
    explicit Polynomial(FF constant);  // Constant polynomial

    // Basic operations
    bool is_zero() const { return coeffs.empty(); }
    bool is_one() const  { return coeffs.size() == 1 && coeffs[0] == FF::one(); }
    bool is_x() const    { return coeffs.size() == 2 && coeffs[0].is_zero() && coeffs[1] == FF::one(); }

    // Degree and coefficients
    const std::vector<FF>& coefficients() const { return coeffs; };
    int64_t degree() const { return coeffs.empty() ? -1 : static_cast<int64_t>(coeffs.size() - 1); }
    std::optional<FF> leading_coefficient() const { return coeffs.empty() ? std::optional<FF>() : std::optional<FF>(coeffs.back()); }

    // Core functionality
    Polynomial<FF> formal_derivative() const;

    // Evaluation
    FF evaluate(const FF& x) const;
    template<typename XF>
    XF evaluate(const XF& x) const;
    std::vector<FF> batch_evaluate(const std::vector<FF>& domain) const;
    std::vector<FF> par_batch_evaluate(const std::vector<FF>& domain) const;

    // Arithmetic operations
    Polynomial<FF> operator+(const Polynomial<FF>& other) const;
    Polynomial<FF> operator-(const Polynomial<FF>& other) const;
    template<typename XF>
    Polynomial<FF> operator*(const Polynomial<XF>& other) const;
    Polynomial<FF> operator/(const Polynomial<FF>& other) const;
    Polynomial<FF> operator%(const Polynomial<FF>& other) const;

    // Comparison operators
    template<typename XF>
    bool operator==(const Polynomial<XF>& other) const;
    template<typename XF>
    bool operator!=(const Polynomial<XF>& other) const { return !(*this == other); }

    // Advanced operations
    template<typename XF>
    Polynomial<FF> scale(const XF& alpha) const;
    Polynomial<FF> pow(uint32_t exponent) const;
    Polynomial<FF> square() const;
    std::pair<Polynomial<FF>, Polynomial<FF>> divide(const Polynomial<FF>& other) const;
    Polynomial<FF> reduce(const Polynomial<FF>& modulus) const;

    // Static factory methods
    static Polynomial<FF> zero();
    static Polynomial<FF> one();
    static Polynomial<FF> from_constant(FF value);
    static Polynomial<FF> x_minus_const(FF value);
    static Polynomial<FF> x_to_the(uint64_t n);
    static Polynomial<FF> interpolate(const std::vector<FF>& domain, const std::vector<FF>& values);
    static Polynomial<FF> zerofier(const std::vector<FF>& roots);

    // Coset operations
    std::vector<FF> fast_coset_evaluate(const FF& offset, size_t order) const;
    static Polynomial<FF> fast_coset_interpolate(const FF& offset, const std::vector<FF>& values);

    // Clean Division
    Polynomial<FF> clean_divide(const Polynomial<FF>& divisor) const;

    // Parallelized versions
    static Polynomial<FF> par_interpolate(const std::vector<FF>& domain, const std::vector<FF>& values);
    static Polynomial<FF> par_zerofier(const std::vector<FF>& roots);

    // Extended GCD
    static std::tuple<Polynomial<FF>, Polynomial<FF>, Polynomial<FF>> xgcd(
        const Polynomial<FF>& x, const Polynomial<FF>& y);

    // Convert to string representation
    std::string to_string() const;

    // Return a polynomial that owns its coefficients. Clones the coefficients
    // if they are not already owned.
    Polynomial<FF> into_owned() const;
    void normalize();

private:
    std::vector<FF> coeffs;
};

// Allow multiplication by field elements from either side
template<typename FF>
Polynomial<FF> operator*(const FF& scalar, const Polynomial<FF>& poly);

template<typename FF>
Polynomial<FF> operator*(const Polynomial<FF>& poly, const FF& scalar);

// Explicit instantiations
extern template class Polynomial<BFieldElement>;
extern template class Polynomial<XFieldElement>;

} // namespace tip5xx

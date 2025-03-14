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

#include "tip5xx/polynomial.hpp"

namespace tip5xx {

template<typename FF>
Polynomial<FF>::Polynomial() : coeffs() {
    // Zero polynomial has no coefficients
}

template<typename FF>
Polynomial<FF>::Polynomial(const std::vector<FF>& coefficients) : coeffs(coefficients) {
    normalize();
}

template<typename FF>
Polynomial<FF>::Polynomial(FF constant) {
    if (!constant.is_zero()) {
        coeffs.push_back(constant);
    }
}

template<typename FF>
void Polynomial<FF>::normalize() {
    while (!coeffs.empty() && coeffs.back().is_zero()) {
        coeffs.pop_back();
    }
}

template<typename FF>
std::tuple<Polynomial<FF>, Polynomial<FF>, Polynomial<FF>> Polynomial<FF>::xgcd(
    const Polynomial<FF>& a, const Polynomial<FF>& b) {
    if (b.is_zero()) {
        // If b is zero, the greatest common divisor is a, and additional coefficients are (1, 0)
        return std::make_tuple(
            a.into_owned(),
            Polynomial<FF>(FF::one()),
            Polynomial<FF>()
        );
    }

    // Use a.divide(b) to obtain both quotient and remainder
    auto [quotient, remainder] = a.divide(b);

    // Recursive call - obtain GCD and Bézout coefficients for next iteration
    auto [gcd, s, t] = xgcd(b, remainder);

    // Compute new Bézout coefficients
    auto new_s = t.into_owned();  // s' = t
    auto new_t = s - quotient * t;  // t' = s - q * t

    return std::make_tuple(gcd, new_s, new_t);
}

template<typename FF>
Polynomial<FF> Polynomial<FF>::operator+(const Polynomial<FF>& other) const {
    auto max_len = std::max(coeffs.size(), other.coefficients().size());
    std::vector<FF> summed(max_len);

    for (size_t i = 0; i < max_len; ++i) {
        FF a = i < coeffs.size() ? coeffs[i] : FF::zero();
        FF b = i < other.coefficients().size() ? other.coefficients()[i] : FF::zero();
        summed[i] = a + b;
    }

    return Polynomial<FF>(summed);
}

template<typename FF>
Polynomial<FF> Polynomial<FF>::operator-(const Polynomial<FF>& other) const {
    auto max_len = std::max(coeffs.size(), other.coefficients().size());
    std::vector<FF> diff(max_len);

    for (size_t i = 0; i < max_len; ++i) {
        FF a = i < coeffs.size() ? coeffs[i] : FF::zero();
        FF b = i < other.coefficients().size() ? other.coefficients()[i] : FF::zero();
        diff[i] = a - b;
    }

    return Polynomial<FF>(diff);
}

template<typename FF>
template<typename XF>
Polynomial<FF> Polynomial<FF>::operator*(const Polynomial<XF>& other) const {
    if (this->degree() < 0 || other.degree() < 0) {
        return Polynomial<FF>();
    }

    const auto degree_lhs = coeffs.size();
    const auto degree_rhs = other.coefficients().size();

    std::vector<FF> product(degree_lhs + degree_rhs - 1, FF::zero());

    for (size_t i = 0; i < degree_lhs; ++i) {
        for (size_t j = 0; j < degree_rhs; ++j) {
            product[i + j] += coeffs[i] * other.coefficients()[j];
        }
    }

    return Polynomial<FF>(product);
}

template<typename FF>
Polynomial<FF> Polynomial<FF>::operator/(const Polynomial<FF>& other) const {
    auto [quotient, _] = this->divide(other);
    return quotient;
}

template<typename FF>
Polynomial<FF> Polynomial<FF>::operator%(const Polynomial<FF>& other) const {
    auto [_, remainder] = this->divide(other);
    return remainder;
}

template<typename FF>
std::pair<Polynomial<FF>, Polynomial<FF>> Polynomial<FF>::divide(const Polynomial<FF>& divisor) const {
    if (divisor.is_zero()) {
        throw std::invalid_argument("Cannot divide by zero polynomial");
    }

    Polynomial<FF> quotient;
    Polynomial<FF> remainder = *this;
    const auto divisor_deg = divisor.degree();
    const auto divisor_lc = divisor.leading_coefficient().value();

    while (!remainder.is_zero() && remainder.degree() >= divisor_deg) {
        const auto deg_diff = remainder.degree() - divisor_deg;
        const auto coeff = remainder.leading_coefficient().value() / divisor_lc;

        std::vector<FF> term_coeffs(deg_diff + 1, FF::zero());
        term_coeffs.back() = coeff;
        Polynomial<FF> term(term_coeffs);

        quotient = quotient + term;
        remainder = remainder - (term * divisor);
    }

    return {quotient, remainder};
}

template<typename FF>
template<typename XF>
bool Polynomial<FF>::operator==(const Polynomial<XF>& other) const {
    if (this->degree() != other.degree()) {
        return false;
    }

    for (size_t i = 0; i < coeffs.size(); ++i) {
        // Handle different field element type combinations
        if constexpr (std::is_same_v<FF, XFieldElement> && std::is_same_v<XF, BFieldElement>) {
            // FF is XFieldElement and XF is BFieldElement
            // Need to compare XFieldElement with lifted BFieldElement
            if (coeffs[i] != other.coefficients()[i].lift()) {
                return false;
            }
        }
        else if constexpr (std::is_same_v<FF, BFieldElement> && std::is_same_v<XF, XFieldElement>) {
            // FF is BFieldElement and XF is XFieldElement
            // Need to compare lifted BFieldElement with XFieldElement
            if (coeffs[i].lift() != other.coefficients()[i]) {
                return false;
            }
        }
        else {
            // Same types - direct comparison
            if (coeffs[i] != other.coefficients()[i]) {
                return false;
            }
        }
    }
    return true;
}

template<typename FF>
FF Polynomial<FF>::evaluate(const FF& x) const {
    FF acc = FF::zero();
    for (auto it = coeffs.rbegin(); it != coeffs.rend(); ++it) {
        acc = acc * x + *it;
    }
    return acc;
}

template<typename FF>
template<typename XF>
XF Polynomial<FF>::evaluate(const XF& x) const {
    XF acc = XF::zero();
    for (auto it = coeffs.rbegin(); it != coeffs.rend(); ++it) {
        acc = acc * x + it->lift();       // tried to us non-existing XF(*it); (Convert FF to XF)
    }
    return acc;
}

template<typename FF>
Polynomial<FF> operator*(const FF& scalar, const Polynomial<FF>& poly) {
    std::vector<FF> result;
    result.reserve(poly.coefficients().size());

    for (const auto& coeff : poly.coefficients()) {
        result.push_back(scalar * coeff);
    }

    return Polynomial<FF>(result);
}

template<typename FF>
Polynomial<FF> operator*(const Polynomial<FF>& poly, const FF& scalar) {
    return scalar * poly;  // Reuse left multiplication
}

template<typename FF>
template<typename XF>
Polynomial<FF> Polynomial<FF>::scale(const XF& alpha) const {
    std::vector<FF> scaled_coeffs;
    scaled_coeffs.reserve(coeffs.size());

    FF power = FF::one();
    for (const auto& coeff : coeffs) {
        scaled_coeffs.push_back(coeff * power);
        power = power * alpha;
    }

    return Polynomial<FF>(scaled_coeffs);
}

template<typename FF>
Polynomial<FF> Polynomial<FF>::pow(uint32_t exponent) const {
    if (exponent == 0) {
        // Using Polynomial() constructor with FF::one()
        return Polynomial<FF>(FF::one());
    }
    if (this->degree() < 0) {
        return Polynomial<FF>();  // Zero polynomial
    }

    // Square-and-multiply algorithm
    auto result = Polynomial<FF>(FF::one());
    auto base = *this;

    while (exponent > 0) {
        if (exponent & 1) {
            result = result * base;
        }
        base = base.square();
        exponent >>= 1;
    }

    return result;
}

template<typename FF>
Polynomial<FF> Polynomial<FF>::square() const {
    if (this->degree() < 0) {
        return Polynomial<FF>();  // Zero polynomial
    }
    if (this->degree() == 0) {
        return Polynomial<FF>(coeffs[0] * coeffs[0]);  // Square constant
    }

    std::vector<FF> squared_coeffs(2 * coeffs.size() - 1, FF::zero());

    // Handle diagonal terms (i=j)
    for (size_t i = 0; i < coeffs.size(); ++i) {
        squared_coeffs[2 * i] += coeffs[i] * coeffs[i];
    }

    // Handle cross terms (i≠j)
    for (size_t i = 0; i < coeffs.size(); ++i) {
        for (size_t j = i + 1; j < coeffs.size(); ++j) {
            squared_coeffs[i + j] += bfe_from(2u) * coeffs[i] * coeffs[j];
        }
    }

    return Polynomial<FF>(squared_coeffs);
}

template<typename FF>
Polynomial<FF> Polynomial<FF>::reduce(const Polynomial<FF>& modulus) const {
    if (modulus.is_zero()) {
        throw std::invalid_argument("Cannot reduce modulo zero polynomial");
    }
    return *this % modulus;
}

template<typename FF>
Polynomial<FF> Polynomial<FF>::zero() {
    return Polynomial<FF>();
}

template<typename FF>
Polynomial<FF> Polynomial<FF>::one() {
    return Polynomial<FF>(FF::one());
}

template<typename FF>
/* static */ Polynomial<FF> Polynomial<FF>::from_constant(FF value) {
    return Polynomial<FF>(value);
}

template<typename FF>
/* static */ Polynomial<FF> Polynomial<FF>::x_minus_const(FF value) {
    std::vector<FF> coeffs = {-value, FF::one()};
    return Polynomial<FF>(coeffs);
}

template<typename FF>
Polynomial<FF> Polynomial<FF>::x_to_the(uint64_t n) {
    std::vector<FF> coeffs(n + 1, FF::zero());
    coeffs[n] = FF::one();
    return Polynomial<FF>(coeffs);
}

template<typename FF>
Polynomial<FF> Polynomial<FF>::zerofier(const std::vector<FF>& roots) {
    static const size_t FAST_ZEROFIER_CUTOFF_THRESHOLD = 100;

    if (roots.empty()) {
        return Polynomial<FF>::one();
    }

    if (roots.size() < FAST_ZEROFIER_CUTOFF_THRESHOLD) {
        // Smart zerofier for small domains
        std::vector<FF> coeffs(roots.size() + 1, FF::zero());
        coeffs[0] = FF::one();
        size_t num_coeffs = 1;

        for (const auto& root : roots) {
            for (size_t k = num_coeffs; k > 0; --k) {
                coeffs[k] = coeffs[k-1] - root * coeffs[k];
            }
            coeffs[0] = -root * coeffs[0];
            num_coeffs++;
        }

        return Polynomial<FF>(coeffs);
    } else {
        // Fast zerofier using divide-and-conquer for larger domains
        const size_t mid_point = roots.size() / 2;
        std::vector<FF> left_roots(roots.begin(), roots.begin() + mid_point);
        std::vector<FF> right_roots(roots.begin() + mid_point, roots.end());

        auto left = zerofier(left_roots);
        auto right = zerofier(right_roots);

        return left * right;
    }
}

template<typename FF>
template<typename S>
void Polynomial<FF>::scalar_mul_mut(const S& scalar) {
    if constexpr (std::is_same_v<FF, XFieldElement> && std::is_same_v<S, BFieldElement>) {
        // If FF is XFieldElement and S is BFieldElement, lift the scalar
        for (auto& coefficient : coeffs) {
            coefficient *= scalar.lift();
        }
    }
    else if constexpr (std::is_same_v<FF, BFieldElement> && std::is_same_v<S, XFieldElement>) {
        // If FF is BFieldElement and S is XFieldElement, cannot multiply directly
        // This case should not be instantiated as it's not a valid operation
        throw std::invalid_argument("Cannot multiply BFieldElement coefficients by XFieldElement scalar");
    }
    else {
        // Same types or other valid combinations
        for (auto& coefficient : coeffs) {
            coefficient *= scalar;
        }
    }
}

template<typename FF>
bool Polynomial<FF>::are_colinear_3(std::pair<FF,FF> p0, std::pair<FF,FF> p1, std::pair<FF,FF> p2) {
    // Check for equal x-coordinates, which would make the slope undefined
    if (p0.first == p1.first || p1.first == p2.first || p2.first == p0.first) {
        return false;
    }

    // Calculate slope between p0 and p1
    FF dy = p0.second - p1.second;
    FF dx = p0.first - p1.first;

    // Use slope comparison: (y2-y0)/(x2-x0) == (y1-y0)/(x1-x0)
    // Written as: dx*(y2-y0) == dy*(x2-x0) to avoid division
    return dx * (p2.second - p0.second) == dy * (p2.first - p0.first);
}

template<typename FF>
bool Polynomial<FF>::are_colinear(const std::vector<std::pair<FF,FF>>& points) {
    if (points.size() < 3) {
        return false;
    }

    // Check for unique x-coordinates
    for (size_t i = 0; i < points.size(); i++) {
        for (size_t j = i + 1; j < points.size(); j++) {
            if (points[i].first == points[j].first) {
                return false;
            }
        }
    }

    // Find 1st degree polynomial through first two points
    const auto& p0 = points[0];
    const auto& p1 = points[1];
    FF a = (p0.second - p1.second) / (p0.first - p1.first);
    FF b = p0.second - a * p0.first;

    // Check if all other points lie on this line
    for (size_t i = 2; i < points.size(); ++i) {
        if (a * points[i].first + b != points[i].second) {
            return false;
        }
    }

    return true;
}

template<typename FF>
std::string Polynomial<FF>::to_string() const {
    if (is_zero()) {
        return "0";
    }

    std::string result;
    bool first = true;

    for (int i = static_cast<int>(coeffs.size()) - 1; i >= 0; --i) {
        if (coeffs[i].is_zero()) continue;

        if (!first) {
            result += " + ";
        }

        if (!coeffs[i].is_one() || i == 0) {
            result += coeffs[i].to_string();
        }

        if (i > 0) {
            result += "x";
            if (i > 1) {
                result += "^" + std::to_string(i);
            }
        }

        first = false;
    }

    return result;
}

template<typename FF>
Polynomial<FF> Polynomial<FF>::into_owned() const {
    return Polynomial<FF>(coeffs);
}

// Explicit instantiations
template class Polynomial<BFieldElement>;
template class Polynomial<XFieldElement>;

// Explicit instantiations for are_colinear_3 and are_colinear
template bool Polynomial<BFieldElement>::are_colinear_3(std::pair<BFieldElement,BFieldElement>, std::pair<BFieldElement,BFieldElement>, std::pair<BFieldElement,BFieldElement>);
template bool Polynomial<BFieldElement>::are_colinear(const std::vector<std::pair<BFieldElement,BFieldElement>>&);
template bool Polynomial<XFieldElement>::are_colinear_3(std::pair<XFieldElement,XFieldElement>, std::pair<XFieldElement,XFieldElement>, std::pair<XFieldElement,XFieldElement>);
template bool Polynomial<XFieldElement>::are_colinear(const std::vector<std::pair<XFieldElement,XFieldElement>>&);

// Explicit instantiations of evaluate method
template XFieldElement Polynomial<BFieldElement>::evaluate<XFieldElement>(const XFieldElement&) const;

// Explicit instantiations of operator==
template bool Polynomial<BFieldElement>::operator==<BFieldElement>(const Polynomial<BFieldElement>& other) const;
template bool Polynomial<XFieldElement>::operator==<BFieldElement>(const Polynomial<BFieldElement>& other) const;
template bool Polynomial<BFieldElement>::operator==<XFieldElement>(const Polynomial<XFieldElement>& other) const;
template bool Polynomial<XFieldElement>::operator==<XFieldElement>(const Polynomial<XFieldElement>& other) const;

// Explicit instantiations for free-standing operator*
template Polynomial<BFieldElement> operator*(const BFieldElement& scalar, const Polynomial<BFieldElement>& poly);
template Polynomial<BFieldElement> operator*(const Polynomial<BFieldElement>& poly, const BFieldElement& scalar);
template Polynomial<XFieldElement> operator*(const XFieldElement& scalar, const Polynomial<XFieldElement>& poly);
template Polynomial<XFieldElement> operator*(const Polynomial<XFieldElement>& poly, const XFieldElement& scalar);

// Explicit instantiations of scale method
template Polynomial<BFieldElement> Polynomial<BFieldElement>::scale<BFieldElement>(const BFieldElement&) const;
template Polynomial<BFieldElement> Polynomial<BFieldElement>::scale<XFieldElement>(const XFieldElement&) const;
template Polynomial<XFieldElement> Polynomial<XFieldElement>::scale<BFieldElement>(const BFieldElement&) const;
template Polynomial<XFieldElement> Polynomial<XFieldElement>::scale<XFieldElement>(const XFieldElement&) const;

// Explicit instantiations of scalar_mul_mut method
template void Polynomial<BFieldElement>::scalar_mul_mut<BFieldElement>(const BFieldElement&);
template void Polynomial<BFieldElement>::scalar_mul_mut<XFieldElement>(const XFieldElement&);
template void Polynomial<XFieldElement>::scalar_mul_mut<BFieldElement>(const BFieldElement&);
template void Polynomial<XFieldElement>::scalar_mul_mut<XFieldElement>(const XFieldElement&);

// Explicit instantiations of to_string method
template std::string Polynomial<BFieldElement>::to_string() const;
template std::string Polynomial<XFieldElement>::to_string() const;


} // namespace tip5xx

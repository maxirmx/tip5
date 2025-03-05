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

#include <algorithm>
#include <array>
#include <bit>
#include <climits>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>


namespace tip5xx {

class XFieldElement;  // Forward declaration

/**
 * Base field element ∈ ℤ_{2^64 - 2^32 + 1}.
 *
 * In Montgomery representation. This implementation follows
 * https://eprint.iacr.org/2022/274.pdf
 */
class BFieldElement {
public:
    static constexpr size_t BYTES = 8;

    // The base field's prime, i.e., 2^64 - 2^32 + 1
    static constexpr uint64_t P = 0xFFFFFFFF00000001ULL;
    static constexpr uint64_t MAX = P - 1;

    // 2^128 mod P; used for conversion of elements into Montgomery representation
    static constexpr uint64_t R2 = 0xFFFFFFFE00000001ULL;

    // -2^-1
    static const BFieldElement MINUS_TWO_INVERSE;

    // Constants
    static const BFieldElement ZERO;
    static const BFieldElement ONE;

    // Constructors
    BFieldElement() : value_(0) {}

    // Main constructor - converts to Montgomery form
    static constexpr BFieldElement new_element(uint64_t value) {
        return BFieldElement(montyred(static_cast<__uint128_t>(value) * static_cast<__uint128_t>(R2)));
    }

    // Try to create new field element if value is canonical, throws std::runtime_error if value is not canonical
    static BFieldElement try_new(uint64_t v);

    // Get canonical representation (out of Montgomery form)
    constexpr uint64_t value() const {
        return canonical_representation();
    }

    // Multiplicative inverse
    BFieldElement inverse() const;
    BFieldElement inverse_or_zero() const;

    // Power accumulator function
    template<size_t N, size_t M>
    static std::array<BFieldElement, N> power_accumulator(
        const std::array<BFieldElement, N>& base,
        const std::array<BFieldElement, N>& tail) {

        std::array<BFieldElement, N> result = base;

        for (size_t i = 0; i < M; i++) {
            for (size_t j = 0; j < N; j++) {
                result[j] = BFieldElement(montyred(
                    static_cast<__uint128_t>(result[j].value_) *
                    static_cast<__uint128_t>(result[j].value_)
                ));
            }
        }

        for (size_t j = 0; j < N; j++) {
            result[j] = BFieldElement(montyred(
                static_cast<__uint128_t>(result[j].value_) *
                static_cast<__uint128_t>(tail[j].value_)
            ));
        }

        return result;
    }

    // Generator of the field
    static BFieldElement generator() {
        return new_element(7UL);
    }

    // Lift to XFieldElement
    XFieldElement lift() const;

    // Testing utilities
    void increment() {
        *this += ONE;
    }

    void decrement() {
        *this -= ONE;
    }

    // Montgomery exponentiation
    BFieldElement mod_pow(uint64_t exp) const;
    BFieldElement mod_pow_u32(uint32_t exp) const;
    BFieldElement mod_pow_u64(uint64_t exp) const;

    // Montgomery reduction
    // Verification correction factor
    static_assert(1 + ~BFieldElement::P == 0x00000000FFFFFFFFULL, "Correction factor calculation is incorrect");

    static constexpr uint64_t montyred(__uint128_t x) {
        // Extract the low and high 64 bits
        uint64_t xl = static_cast<uint64_t>(x);
        uint64_t xh = static_cast<uint64_t>(x >> 64);

        // Manual overflow detection for xl + (xl << 32)
        uint64_t shifted = xl << 32;
        uint64_t a = xl + shifted;
        bool e = (a < xl) || (a < shifted);  // Overflow occurred if result is less than either operand

        // Wrapping subtraction operations
        uint64_t b = a - (a >> 32);
        if (e) b--;  // Subtract 1 if there was overflow

        // Manual underflow detection for xh - b
        uint64_t r = xh - b;
        bool c = xh < b;  // Underflow occurred if xh < b
        return r - ((1 + ~P) * (c ? 1ULL : 0ULL));
    }

    static uint64_t montyred_nc(__uint128_t x) {
        // See reference above for a description of the following implementation.
        uint64_t xl = static_cast<uint64_t>(x);
        uint64_t xh = static_cast<uint64_t>(x >> 64);

        // Equivalent to Rust's overflowing_add
        uint64_t a;
        bool e;
        if (__builtin_add_overflow(xl, xl << 32, &a)) {
            e = true;
        } else {
            e = false;
        }

        // Wrapping subtraction in C++
        uint64_t b = a - (a >> 32);
        if (e) b--;  // Equivalent to wrapping_sub(e as u64)

        // Equivalent to Rust's overflowing_sub
        uint64_t r;
        bool c;
        if (__builtin_sub_overflow(xh, b, &r)) {
            c = true;
        } else {
            c = false;
        }

        // Final correction
        // Note: P is a class constant in the C++ version
        return r - ((1 + ~P) * (c ? 1 : 0));
    }

    // Raw byte conversions
    std::array<uint8_t, 8> raw_bytes() const;
    static BFieldElement from_raw_bytes(const std::array<uint8_t, 8>& bytes);

    // Raw 16-bit conversions
    std::array<uint16_t, 4> raw_u16s() const;
    static BFieldElement from_raw_u16s(const std::array<uint16_t, 4>& chunks);

    // Raw value accessors
    __uint128_t raw_u128() const {
        return static_cast<__uint128_t>(value_);
    }

    static BFieldElement from_raw_u64(uint64_t e) {
        return BFieldElement(e);
    }

    uint64_t raw_u64() const {
        return value_;
    }

    // Check if a value is canonical (within field range)
    static constexpr bool is_canonical(uint64_t x) {
        return x < P;
    }

    // Square the value
    BFieldElement square() const {
        return *this * *this;
    }

    // Is zero or one checks
    bool is_zero() const {
        return *this == ZERO;
    }

    bool is_one() const {
        return *this == ONE;
    }

    // Batch inversion of multiple elements
    static std::vector<BFieldElement> batch_inversion(const std::vector<BFieldElement>& elements);

    // Primitive root of unity - throws std::runtime_error if root does not exist
    static BFieldElement primitive_root_of_unity(uint64_t n);

    // Get cyclic group elements
    std::vector<BFieldElement> cyclic_group_elements(size_t max = 0) const;

    // Arithmetic operators
    BFieldElement operator+(const BFieldElement& rhs) const;
    BFieldElement& operator+=(const BFieldElement& rhs);

    BFieldElement operator-(const BFieldElement& rhs) const;
    BFieldElement& operator-=(const BFieldElement& rhs);

    BFieldElement operator*(const BFieldElement& rhs) const;
    BFieldElement& operator*=(const BFieldElement& rhs);

    BFieldElement operator/(const BFieldElement& rhs) const;

    BFieldElement operator-() const;

    // Equality and comparison
    bool operator==(const BFieldElement& rhs) const {
        return value() == rhs.value();
    }

    bool operator!=(const BFieldElement& rhs) const {
        return !(*this == rhs);
    }

    // Convert to string representation
    std::string to_string() const;

    // Explicit conversion methods that can throw exceptions for types that might not fit
    template <typename T>
    T to() const {
        uint64_t val = canonical_representation();
        if constexpr (std::is_unsigned_v<T>) {
            if (val > std::numeric_limits<T>::max()) {
                throw std::overflow_error("BFieldElement value too large for target type.");
            }
            return static_cast<T>(val);
        }
        else if constexpr (std::is_signed_v<T>) {
            if (val > P / 2) {  // This is a negative value in the field
                int64_t signed_val = static_cast<int64_t>(val) - static_cast<int64_t>(P);
                if (signed_val < std::numeric_limits<T>::min()) {
                    throw std::underflow_error("BFieldElement value too small for target type.");
                }
                return static_cast<T>(signed_val);
            }
            else {  // This is a positive value
                if (val > std::numeric_limits<T>::max()) {
                    throw std::overflow_error("BFieldElement value too large for target type.");
                }
                return static_cast<T>(val);
            }
        }
    }
    // Explicit cast operators for small integer types that might fail
    explicit operator uint8_t() const { return to<uint8_t>(); }
    explicit operator int8_t() const { return to<int8_t>(); }
    explicit operator uint16_t() const { return to<uint16_t>(); }
    explicit operator int16_t() const { return to<int16_t>(); }
    explicit operator uint32_t() const { return to<uint32_t>(); }
    explicit operator int32_t() const { return to<int32_t>(); }
    explicit operator int64_t() const { return to<int64_t>(); }

    // Add size_t operator only when it's different from uint64_t (e.g., on macOS)
    #if defined(__APPLE__) || \
        (defined(SIZE_MAX) && defined(UINT64_MAX) && (SIZE_MAX != UINT64_MAX))
    explicit operator size_t() const {
        return static_cast<size_t>(canonical_representation());
    }
    #endif

    // Implicit conversions for types that can always hold a BFieldElement value
    operator uint64_t() const {
        return canonical_representation();
    }

    operator __uint128_t() const {
        return static_cast<__uint128_t>(canonical_representation());
    }

    operator __int128_t() const {
        uint64_t val = canonical_representation();
        if (val <= static_cast<uint64_t>(INT64_MAX)) {
            return static_cast<__int128_t>(val);
        } else {
            // Handle negative values
            return static_cast<__int128_t>(val) - static_cast<__int128_t>(P);
        }
    }

    static constexpr uint64_t mod_reduce(__uint128_t x) {
        const uint64_t LOWER_MASK = 0xFFFFFFFF;

        uint64_t x_lo = static_cast<uint64_t>(x);
        uint64_t x_hi = static_cast<uint64_t>(x >> 64);
        uint64_t x_hi_lo = static_cast<uint64_t>(static_cast<uint32_t>(x_hi));
        uint64_t x_hi_hi = x_hi >> 32;

        // Detect underflow manually
        uint64_t tmp0 = x_lo - x_hi_hi;
        bool is_underflow = x_lo < x_hi_hi;

        uint64_t tmp1 = tmp0 - (LOWER_MASK * (is_underflow ? 1 : 0));
        uint64_t tmp2 = (x_hi_lo << 32) - x_hi_lo;

        // Detect overflow manually
        uint64_t result = tmp1 + tmp2;
        bool is_overflow = (result < tmp1) || (result < tmp2);

        // Apply overflow correction if needed
        return result + (LOWER_MASK * (is_overflow ? 1 : 0));
    }
private:
    uint64_t value_;

    // Constructor from raw value (already in Montgomery form)
    explicit constexpr BFieldElement(uint64_t value) : value_(value) {}

    constexpr uint64_t canonical_representation() const {
        return montyred(static_cast<__uint128_t>(value_));
    }


    // Internal exponentiation helper
    static BFieldElement exp(BFieldElement base, uint64_t exponent);

    // Primitive roots lookup table
    static const std::unordered_map<uint64_t, uint64_t> PRIMITIVE_ROOTS;
};

// Stream operators
std::ostream& operator<<(std::ostream& os, const BFieldElement& bfe);
std::istream& operator>>(std::istream& is, BFieldElement& bfe);

// Parse from string
BFieldElement bfe_from_string(const std::string& str);
BFieldElement bfe_from_hex_string(const std::string& str);

// Macros to simplify creation
#define bfe(x) BFieldElement::new_element(x)
#define bfe_vec(...) createBfeVec(__VA_ARGS__)
#define bfe_array(...) createBfeArray(__VA_ARGS__)

// Helper functions for macros
template<typename... Args>
std::vector<BFieldElement> createBfeVec(Args... args);

template<size_t N, typename... Args>
std::array<BFieldElement, N> createBfeArray(Args... args);

// Template method for integer types (both signed and unsigned)
template <typename T>
static BFieldElement bfe_from(T value) {
    static_assert(std::is_integral_v<T>, "bfe_from only works with integer types");

    if constexpr (std::is_signed_v<T>) {
        if (value < 0) {
            // For negative values, add P to get the field equivalent
            return BFieldElement::new_element(static_cast<uint64_t>(BFieldElement::P + static_cast<int64_t>(value)));
        }
    }
    return BFieldElement::new_element(static_cast<uint64_t>(value));
}

static BFieldElement bfe_from(__uint128_t value) {
    return BFieldElement::new_element(BFieldElement::mod_reduce(value)); }


// Constructor for int64_t values
static BFieldElement bfe_from(int64_t value) {
    if (value >= 0) {
        // Non-negative case - directly convert to u128
        return bfe_from(static_cast<__uint128_t>(value));
    } else {
        // Negative case - subtract R2 from the unsigned value
        // This matches the Rust implementation's handling of negative values
        return bfe_from(static_cast<__uint128_t>(value) - static_cast<__uint128_t>(BFieldElement::R2));
    }
}

// Helper functions to make conversions more explicit when needed
template <typename T>
T bfe_to(const BFieldElement& bfe) {
    return bfe.to<T>();
}

template <typename T>
bool try_bfe_to(const BFieldElement& bfe, T& output) {
    try {
        output = bfe.to<T>();
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

} // namespace tip5xx

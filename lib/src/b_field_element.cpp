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

#include "tip5xx/b_field_element.hpp"

namespace tip5xx {

// Initialize static constants
const BFieldElement BFieldElement::ZERO = BFieldElement::new_element(0UL);
const BFieldElement BFieldElement::ONE = BFieldElement::new_element(1UL);
const BFieldElement BFieldElement::MINUS_TWO_INVERSE = BFieldElement::new_element(0x7FFFFFFF80000000);

// Primitive roots lookup table
const std::unordered_map<uint64_t, uint64_t> BFieldElement::PRIMITIVE_ROOTS = {
    {0, 1},
    {1, 1},
    {2, 18446744069414584320ULL},
    {4, 281474976710656ULL},
    {8, 18446744069397807105ULL},
    {16, 17293822564807737345ULL},
    {32, 70368744161280ULL},
    {64, 549755813888ULL},
    {128, 17870292113338400769ULL},
    {256, 13797081185216407910ULL},
    {512, 1803076106186727246ULL},
    {1024, 11353340290879379826ULL},
    {2048, 455906449640507599ULL},
    {4096, 17492915097719143606ULL},
    {8192, 1532612707718625687ULL},
    {16384, 16207902636198568418ULL},
    {32768, 17776499369601055404ULL},
    {65536, 6115771955107415310ULL},
    {131072, 12380578893860276750ULL},
    {262144, 9306717745644682924ULL},
    {524288, 18146160046829613826ULL},
    {1048576, 3511170319078647661ULL},
    {2097152, 17654865857378133588ULL},
    {4194304, 5416168637041100469ULL},
    {8388608, 16905767614792059275ULL},
    {16777216, 9713644485405565297ULL},
    {33554432, 5456943929260765144ULL},
    {67108864, 17096174751763063430ULL},
    {134217728, 1213594585890690845ULL},
    {268435456, 6414415596519834757ULL},
    {536870912, 16116352524544190054ULL},
    {1073741824, 9123114210336311365ULL},
    {2147483648, 4614640910117430873ULL},
    {4294967296, 1753635133440165772ULL}
};

// Try to create new field element if value is canonical
BFieldElement BFieldElement::try_new(uint64_t v) {
    if (!is_canonical(v)) {
        throw ParseBFieldElementError(ParseBFieldElementError::ErrorType::NotCanonical);
    }
    return new_element(v);
}

// Implementation of Inverse trait
BFieldElement BFieldElement::inverse_impl() const {
    if (is_zero()) {
        throw BFieldElementInverseError();
    }
    const BFieldElement& x = *this;

    BFieldElement bin_2_ones = (*this) * (*this) * x;
    BFieldElement bin_3_ones = (bin_2_ones * bin_2_ones) * x;
    BFieldElement bin_6_ones = exp(bin_3_ones, 3) * bin_3_ones;
    BFieldElement bin_12_ones = exp(bin_6_ones, 6) * bin_6_ones;
    BFieldElement bin_24_ones = exp(bin_12_ones, 12) * bin_12_ones;
    BFieldElement bin_30_ones = exp(bin_24_ones, 6) * bin_6_ones;
    BFieldElement bin_31_ones = (bin_30_ones * bin_30_ones) * x;
    BFieldElement bin_31_ones_1_zero = bin_31_ones * bin_31_ones;
    BFieldElement bin_32_ones = (bin_31_ones * bin_31_ones) * x;

    return exp(bin_31_ones_1_zero, 32) * bin_32_ones;
}

// Internal exponentiation helper
BFieldElement BFieldElement::exp(BFieldElement base, uint64_t exponent) {
    BFieldElement res = base;
    for (uint64_t i = 0; i < exponent; i++) {
        res = res * res;
    }
    return res;
}

// Montgomery exponentiation
BFieldElement BFieldElement::mod_pow(uint64_t exp) const {
    BFieldElement acc = ONE;

    // Handle countl_zero - determine bit length without using std::countl_zero
    int bit_length = 0;
    uint64_t temp = exp;
    while (temp != 0) {
        temp >>= 1;
        bit_length++;
    }

    for (int i = 0; i < bit_length; i++) {
        acc = acc * acc;

        if (exp & (1ULL << (bit_length - 1 - i))) {
            acc = acc * (*this);
        }
    }

    return acc;
}

BFieldElement BFieldElement::mod_pow_u32(uint32_t exp) const {
    return mod_pow(static_cast<uint64_t>(exp));
}

BFieldElement BFieldElement::mod_pow_u64(uint64_t exp) const {
    return mod_pow(exp);
}

// Raw byte conversions
std::array<uint8_t, 8> BFieldElement::raw_bytes() const {
    std::array<uint8_t, 8> result;
    uint64_t val = value();

    for (int i = 0; i < 8; i++) {
        result[i] = static_cast<uint8_t>(val & 0xFF);
        val >>= 8;
    }

    return result;
}

BFieldElement BFieldElement::from_raw_bytes(const std::array<uint8_t, 8>& bytes) {
    uint64_t result = 0;

    for (int i = 7; i >= 0; i--) {
        result = (result << 8) | bytes[i];
    }

    return try_new(result);
}

// Raw 16-bit conversions
std::array<uint16_t, 4> BFieldElement::raw_u16s() const {
    std::array<uint16_t, 4> result;
    uint64_t val = value();

    for (int i = 0; i < 4; i++) {
        result[i] = static_cast<uint16_t>(val & 0xFFFF);
        val >>= 16;
    }

    return result;
}

BFieldElement BFieldElement::from_raw_u16s(const std::array<uint16_t, 4>& chunks) {
    uint64_t result = 0;

    for (int i = 3; i >= 0; i--) {
        result = (result << 16) | chunks[i];
    }

    return try_new(result);
}

// Implementation of PrimitiveRootOfUnity trait
BFieldElement BFieldElement::primitive_root_of_unity_impl(uint64_t n) {
    auto it = PRIMITIVE_ROOTS.find(n);
    if (it == PRIMITIVE_ROOTS.end()) {
        throw BFieldElementPrimitiveRootError();
    }
    return new_element(it->second);
}

// Implementation of ModPowU64 trait
BFieldElement BFieldElement::mod_pow_u64_impl(uint64_t exp) const {
    return mod_pow(exp);
}

// Implementation of ModPowU32 trait
BFieldElement BFieldElement::mod_pow_u32_impl(uint32_t exp) const {
    return mod_pow(static_cast<uint64_t>(exp));
}

// Implementation of CyclicGroupGenerator trait
std::vector<BFieldElement> BFieldElement::cyclic_group_elements_impl(size_t max) const {
    // Special case for zero
    if (is_zero()) {
        return {ZERO};
    }

    BFieldElement val = *this;
    std::vector<BFieldElement> result = {ONE};

    while (!val.is_one() && (max == 0 || result.size() < max)) {
        result.push_back(val);
        val *= *this;
    }

    return result;
}

// Addition operator
BFieldElement BFieldElement::operator+(const BFieldElement& rhs) const {
    // Compute a + b = a - (p - b)
    uint64_t x1;
    bool c1 = __builtin_sub_overflow(value_, P - rhs.value_, &x1);

    if (c1) {
        return BFieldElement(x1 + P);
    } else {
        return BFieldElement(x1);
    }
}

BFieldElement& BFieldElement::operator+=(const BFieldElement& rhs) {
    *this = *this + rhs;
    return *this;
}

// Subtraction operator
BFieldElement BFieldElement::operator-(const BFieldElement& rhs) const {
    uint64_t x1;
    bool c1 = __builtin_sub_overflow(value_, rhs.value_, &x1);

    return BFieldElement(x1 - ((1 + ~P) * (c1 ? 1 : 0)));
}

BFieldElement& BFieldElement::operator-=(const BFieldElement& rhs) {
    *this = *this - rhs;
    return *this;
}

// Multiplication operator
BFieldElement BFieldElement::operator*(const BFieldElement& rhs) const {
    return BFieldElement(montyred(
        static_cast<__uint128_t>(value_) *
        static_cast<__uint128_t>(rhs.value_)
    ));
}

BFieldElement& BFieldElement::operator*=(const BFieldElement& rhs) {
    *this = *this * rhs;
    return *this;
}

// Division operator
BFieldElement BFieldElement::operator/(const BFieldElement& rhs) const {
    return *this * rhs.inverse_impl();
}

// Negation operator
BFieldElement BFieldElement::operator-() const {
    return ZERO - *this;
}

// Stream output operator
std::ostream& operator<<(std::ostream& os, const BFieldElement& bfe) {
    uint64_t canonical_value = bfe.value();
    const uint64_t cutoff = 256;

    if (canonical_value >= BFieldElement::P - cutoff) {
        os << "-" << (BFieldElement::P - canonical_value);
    } else if (canonical_value <= cutoff) {
        os << canonical_value;
    } else {
        os.width(20);
        os.fill('0');
        os << canonical_value;
    }

    return os;
}

// Convert from string representation, handling both positive and negative values
BFieldElement bfe_from_string(const std::string& s) {
    // Trim whitespace
    std::string str = s;
    str.erase(0, str.find_first_not_of(" \t\n\r"));
    str.erase(str.find_last_not_of(" \t\n\r") + 1);

    // Check if empty
    if (str.empty()) {
        throw BFieldElementStringConversionError(BFieldElementStringConversionError::ErrorType::Empty);
    }

    // Handle hex format with prefix
    if (str.size() >= 2 && str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
        return bfe_from_hex_string(str);
    }

    // Handle sign
    bool is_negative = false;
    if (str[0] == '-') {
        is_negative = true;
        str = str.substr(1);
    } else if (str[0] == '+') {
        str = str.substr(1);
    }

    // Parse as __int128_t
    __int128_t parsed = 0;
    for (char c : str) {
        if (!std::isdigit(c)) {
            throw BFieldElementStringConversionError(BFieldElementStringConversionError::ErrorType::InvalidDigit);
        }

        parsed = parsed * 10 + (c - '0');

        // Check for overflow
        if (parsed & ((__int128_t)1 << 126)) {
            throw BFieldElementStringConversionError(BFieldElementStringConversionError::ErrorType::Overflow);
        }
    }

    if (is_negative) {
        parsed = -parsed;
    }

    // Normalize according to field rules
    const __int128_t p = BFieldElement::P;
    __int128_t normalized;

    if (parsed <= -p) {
        throw BFieldElementStringConversionError(BFieldElementStringConversionError::ErrorType::OutOfRange, "too negative");
    } else if (parsed < 0) {
        normalized = parsed + p;
    } else if (parsed >= p) {
        throw BFieldElementStringConversionError(BFieldElementStringConversionError::ErrorType::OutOfRange, "too large");
    } else {
        normalized = parsed;
    }

    // Ensure it fits in uint64_t
    // The value of p is 0xFFFFFFFF00000001 which is close to UINT64_MAX (0xFFFFFFFFFFFFFFFF)
    // For negative values, you add p to normalize
    // If parsed is a large negative value, e.g., -1, then normalized = -1 + p which is just below p
    // Even the most negative allowed value -p + 1 would normalize to (-p + 1) + p = 1
    // So, no, the condition normalized > UINT64_MAX can't be triggered with the current checks in place. The checks parsed <= -p and parsed >= p already prevent any value that would normalize beyond UINT64_MAX.
    //
    // The exception is essentially unreachable because:
    //
    // Any positive parsed must be < p (which is < UINT64_MAX)
    // Any negative parsed must be > -p, and -p + p = 0
    // if (normalized > UINT64_MAX) {
    //    throw std::overflow_error("Normalized value exceeds uint64_t range.");
    // }

    return BFieldElement::new_element(static_cast<uint64_t>(normalized));
}

// Additional helper for hex string parsing
BFieldElement bfe_from_hex_string(const std::string& s) {
    // Remove "0x" prefix if present
    std::string hex = s;
    if (hex.size() >= 2 && hex[0] == '0' && (hex[1] == 'x' || hex[1] == 'X')) {
        hex = hex.substr(2);
    }

    // Check if empty after prefix removal
    if (hex.empty()) {
        throw BFieldElementStringConversionError(BFieldElementStringConversionError::ErrorType::Empty, "hex string");
    }

    // Parse hex string to __uint128_t
    __uint128_t value = 0;
    for (char c : hex) {
        // Check for hex digit
        if (!std::isxdigit(c)) {
            throw BFieldElementStringConversionError(BFieldElementStringConversionError::ErrorType::InvalidHexChar);
        }

        value <<= 4;
        if (c >= '0' && c <= '9') {
            value |= (c - '0');
        } else if (c >= 'a' && c <= 'f') {
            value |= (c - 'a' + 10);
        } else if (c >= 'A' && c <= 'F') {
            value |= (c - 'A' + 10);
        }

        // Check overflow
        if (value >= ((__uint128_t)1 << 127)) {
            throw BFieldElementStringConversionError(BFieldElementStringConversionError::ErrorType::Overflow, "hex value");
        }
    }

    // Create BFieldElement from the value
    return bfe_from(value);
}

// Stream input operator
std::istream& operator>>(std::istream& is, BFieldElement& bfe) {
    std::string str;
    is >> str;

    bfe = bfe_from_string(str);
    return is;
}

std::string BFieldElement::to_string() const {
    std::ostringstream ss;
    uint64_t canonical_value = value();
    const uint64_t cutoff = 256;

    if (canonical_value >= BFieldElement::P - cutoff) {
        ss << "-" << (BFieldElement::P - canonical_value);
    } else if (canonical_value <= cutoff) {
        ss << canonical_value;
    } else {
        ss.width(20);
        ss.fill('0');
        ss << canonical_value;
    }

    return ss.str();
}

// Implement XFieldElement lift
//XFieldElement BFieldElement::lift() const {
//    return XFieldElement::new_const(*this);
//}

} // namespace tip5xx

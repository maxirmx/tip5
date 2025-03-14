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
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>
#include <optional>
#include "tip5xx/b_field_element.hpp"

namespace tip5xx {

class Digest {
public:
    static constexpr size_t LEN = 5;
    static constexpr size_t BYTES = LEN * BFieldElement::BYTES;

    // Constructors
    Digest() = default;
    explicit Digest(const std::array<BFieldElement, LEN>& elements) : elements_(elements) {}

    // Core operations
    Digest reversed() const {
        std::array<BFieldElement, LEN> rev;
        for (size_t i = 0; i < LEN; ++i) {
            rev[i] = elements_[LEN - 1 - i];
        }
        return Digest(rev);
    }

    // Comparison operators
    bool operator>(const Digest& other) const;
    bool operator<(const Digest& other) const;
    bool operator==(const Digest& other) const;
    bool operator!=(const Digest& other) const;
    bool operator<=(const Digest& other) const;
    bool operator>=(const Digest& other) const;

    // Array access operators
    BFieldElement& operator[](size_t index) { return elements_[index]; }
    const BFieldElement& operator[](size_t index) const { return elements_[index]; }

    // String conversion
    std::string to_string() const;
    std::string to_hex() const;
    std::string to_hex_upper() const;
    std::array<uint8_t, BYTES> to_bytes() const;


    // Static factory methods
    static std::optional<Digest> from_string(const std::string& str);
    static std::optional<Digest> from_hex(const std::string& hex_str);
    static std::optional<Digest> from_bytes(const std::array<uint8_t, BYTES>& bytes);
    static Digest try_from_hex(const std::string& hex_str);

    // Hashing functionality
    Digest hash() const;

   // Access to the element array
   const std::array<BFieldElement, LEN>& values() const { return elements_; }
   std::array<BFieldElement, LEN>& mutable_values() { return elements_; }

// Vector/Array Conversion Methods
    static std::optional<Digest> from_bfield_elements(const std::vector<BFieldElement>& elements);
    static std::optional<Digest> from_slice(const BFieldElement* elements, size_t size);
    std::vector<BFieldElement> to_bfield_elements() const;

private:
    std::array<BFieldElement, LEN> elements_ = {BFieldElement()};  // Zero-initialized by default
    static BFieldElement try_bfe_from_hex(const std::string& hex_str);
};

// Parse from string
Digest digest_from_string(const std::string& str);

} // namespace tip5xx

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

#include <sstream>
#include <iomanip>
#include "tip5xx/digest.hpp"
#include "tip5xx/tip5xx.hpp"

namespace tip5xx {

std::string Digest::to_string() const {
    std::ostringstream oss;
    for (size_t i = 0; i < LEN; ++i) {
        if (i > 0) {
            oss << ',';
        }
        oss << elements_[i].to_string();
    }
    return oss.str();
}

// Parse a Digest from a string like "number1,number2,number3,number4,number5"
std::optional<Digest> Digest::from_string(const std::string& str) {
    std::vector<BFieldElement> elements;
    std::istringstream iss(str);
    std::string token;

    // Parse comma-separated values
    while (std::getline(iss, token, ',')) {
        // Try to parse each token as a BFieldElement
        try
        {
            BFieldElement bfe = bfe_from_string(token);
            elements.push_back(bfe);
        }
        catch (const std::exception&)
        {
            return std::nullopt; // Failed to parse a BFieldElement
        }
    }

    // Check if we got the correct number of elements
    if (elements.size() != LEN) {
        return std::nullopt; // Invalid length
    }

    // Convert vector to array
    std::array<BFieldElement, LEN> element_array;
    for (size_t i = 0; i < LEN; ++i) {
        element_array[i] = elements[i];
    }

    return Digest(element_array);
}

std::string Digest::to_hex() const {
    std::ostringstream ss;
    ss << std::hex << std::setfill('0');

    // Process each element individually
    for (const auto& element : elements_) {
        // Format each element as a 16-character hex string
        ss << std::setw(16) << element.value();
    }

    return ss.str();
}

std::string Digest::to_hex_upper() const {
    std::ostringstream oss;
    oss << std::hex << std::uppercase << std::setfill('0');

    for (const auto& element : elements_) {
        uint64_t value = element.value();

        // Ensure each digit is handled individually for consistent uppercase
        for (int i = 0; i < 16; i++) {
            // Process 4 bits at a time, starting from most significant
            uint8_t nibble = (value >> (60 - i*4)) & 0xF;

            // Convert nibble to uppercase hex character
            char hexChar = (nibble < 10) ? ('0' + nibble) : ('A' + (nibble - 10));
            oss << hexChar;
        }
    }

    return oss.str();
}

std::optional<Digest> Digest::from_hex(const std::string& hex_str) {
    // Check if the hex string has the correct length
    // Each BFieldElement is 64 bits = 16 hex chars
    // So we need LEN * 16 hex chars
    if (hex_str.length() != LEN * 16) {
        return std::nullopt;
    }

    std::array<BFieldElement, LEN> elements;

    for (size_t i = 0; i < LEN; ++i) {
        std::string element_hex = hex_str.substr(i * 16, 16);

        // Parse hex string to uint64_t
        uint64_t value = 0;
        try {
            value = std::stoull(element_hex, nullptr, 16);
        } catch (const std::exception&) {
            return std::nullopt; // Invalid hex string
        }

        // Check if the value is canonical (less than modulus)
        if (value >= BFieldElement::MAX_VALUE) {
            return std::nullopt; // Non-canonical value
        }

        elements[i] = BFieldElement::new_element(value);
    }

    return Digest(elements);
}

std::array<uint8_t, Digest::BYTES> Digest::to_bytes() const {
    std::array<uint8_t, BYTES> result;
    for (size_t i = 0; i < LEN; i++) {
        auto bytes = elements_[i].to_bytes();
        std::copy(bytes.begin(), bytes.end(), result.begin() + i * BFieldElement::BYTES);
    }
    return result;
}


std::optional<Digest> Digest::from_bytes(const std::array<uint8_t, BYTES>& bytes) {
    std::array<BFieldElement, LEN> elements;

    for (size_t i = 0; i < LEN; ++i) {
        uint64_t value = 0;

        // Extract 8 bytes for this element and convert to BFieldElement
        std::array<uint8_t, BFieldElement::BYTES> element_bytes;
        std::copy_n(bytes.begin() + i * BFieldElement::BYTES,
                BFieldElement::BYTES,
                element_bytes.begin());

        try {
            elements[i] = BFieldElement::from_bytes(element_bytes);
        } catch (const BFieldElementError&) {
            return std::nullopt; // Invalid byte representation
        }
    }

    return std::optional<Digest>(Digest(elements));
}

BFieldElement Digest::try_bfe_from_hex(const std::string& hex_str) {
    if (hex_str.length() != 16) {
        throw TryFromHexDigestError(TryFromHexDigestError::ErrorType::Digest, "Invalid hex string length");
    }
    uint64_t value = std::stoull(hex_str, nullptr, 16);
    return BFieldElement::new_element(value);
}

Digest Digest::try_from_hex(const std::string& hex_str) {
    if (hex_str.length() != BYTES * 2) {
        throw TryFromHexDigestError(TryFromHexDigestError::ErrorType::Digest, "Invalid hex string length");
    }

    std::array<BFieldElement, LEN> elements;
    for (size_t i = 0; i < LEN; i++) {
        std::string element_hex = hex_str.substr(i * 16, 16);
        elements[i] = try_bfe_from_hex(element_hex);
    }
    return Digest(elements);
}

Digest Digest::hash() const {
    return Tip5::hash_pair(*this, Digest());
}

Digest digest_from_string(const std::string& str) {
    std::stringstream ss(str);
    std::string item;
    std::array<BFieldElement, Digest::LEN> elements;
    size_t i = 0;

    while (std::getline(ss, item, ',')) {
        if (i >= Digest::LEN) {
            throw TryFromDigestError(TryFromDigestError::ErrorType::InvalidLength, "Too many elements in string");
        }
        elements[i++] = bfe_from_string(item);
    }

    if (i != Digest::LEN) {
        throw TryFromDigestError(TryFromDigestError::ErrorType::InvalidLength, "Not enough elements in string");
    }

    return Digest(elements);
}

bool Digest::operator>(const Digest& other) const {
    // Compare elements in reverse order (most significant first)
    for (int i = LEN - 1; i >= 0; --i) {
        // Compare raw values, not BFieldElement objects
        if (elements_[i].value() > other.elements_[i].value()) return true;
        if (elements_[i].value() < other.elements_[i].value()) return false;
    }
    return false; // equal
}

bool Digest::operator>=(const Digest& other) const {
    // Compare elements in reverse order (most significant first)
    for (int i = LEN - 1; i >= 0; --i) {
        // Compare raw values, not BFieldElement objects
        if (elements_[i].value() > other.elements_[i].value()) return true;
        if (elements_[i].value() < other.elements_[i].value()) return false;
    }
    return true; // equal
}

bool Digest::operator<(const Digest& other) const {
    // Compare elements in reverse order (most significant first)
    for (int i = LEN - 1; i >= 0; --i) {
        if (elements_[i].value() < other.elements_[i].value()) return true;
        if (elements_[i].value() > other.elements_[i].value()) return false;
    }
    return false; // equal
}

bool Digest::operator<=(const Digest& other) const {
    // Compare elements in reverse order (most significant first)
    for (int i = LEN - 1; i >= 0; --i) {
        if (elements_[i].value() < other.elements_[i].value()) return true;
        if (elements_[i].value() > other.elements_[i].value()) return false;
    }
    return true; // equal
}

bool Digest::operator==(const Digest& other) const {
    for (size_t i = 0; i < LEN; ++i) {
        if (elements_[i].value() != other.elements_[i].value()) return false;
    }
    return true;
}

bool Digest::operator!=(const Digest& other) const {
    for (size_t i = 0; i < LEN; ++i) {
        if (elements_[i].value() != other.elements_[i].value()) return true;
    }
    return false;
}

std::optional<Digest> Digest::from_bfield_elements(const std::vector<BFieldElement>& elements) {
    // Check if the input vector has the correct size
    if (elements.size() != LEN) {
        return std::nullopt;
    }

    // Convert vector to array
    std::array<BFieldElement, LEN> element_array;
    std::copy(elements.begin(), elements.end(), element_array.begin());

    return Digest(element_array);
}

std::optional<Digest> Digest::from_slice(const BFieldElement* elements, size_t size) {
    // Check if the input slice has the correct size
    if (size != LEN) {
        return std::nullopt;
    }

    // Convert slice to array
    std::array<BFieldElement, LEN> element_array;
    for (size_t i = 0; i < LEN; ++i) {
        element_array[i] = elements[i];
    }

    return Digest(element_array);
}

std::vector<BFieldElement> Digest::to_bfield_elements() const {
    // Convert internal array to vector
    return std::vector<BFieldElement>(elements_.begin(), elements_.end());
}

} // namespace tip5xx

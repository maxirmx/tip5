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

#include "tip5xx/b_field_element_error.hpp"
#include "tip5xx/b_field_element.hpp"

// Constants
static constexpr size_t EXTENSION_DEGREE = 4;
namespace Digest {
    static constexpr size_t LEN = 32;
}

namespace tip5xx {

std::string ParseBFieldElementError::build_message(ErrorType type, const std::string& detail) {
    std::ostringstream oss;
    switch (type) {
        case ErrorType::NotCanonical:
            oss << detail << " must be in canonical (open) interval (-" << BFieldElement::MAX_VALUE << ", " << BFieldElement::MAX_VALUE << ")";
            break;
//        case ErrorType::InvalidNumBytes:
//            oss << "incorrect number of bytes: " << detail << " != " << BFieldElement::BYTES << " == `BFieldElement::BYTES`";
//            break;
    }
    return oss.str();
}

std::string TryFromXFieldElementError::build_message(ErrorType type, const std::string& detail) {
    std::ostringstream oss;
    switch (type) {
        case ErrorType::InvalidLength:
            oss << "expected " << EXTENSION_DEGREE << " elements for extension field element, but got " << detail;
            break;
        case ErrorType::InvalidDigest:
            oss << "Digest is not an XFieldElement";
            break;
    }
    return oss.str();
}

std::string TryFromDigestError::build_message(ErrorType type, const std::string& detail) {
    std::ostringstream oss;
    switch (type) {
        case ErrorType::InvalidLength:
            oss << "expected " << Digest::LEN << " elements for digest, but got " << detail;
            break;
        case ErrorType::InvalidBFieldElement:
            oss << "invalid `BFieldElement`: " << detail;
            break;
        case ErrorType::Overflow:
            oss << "overflow converting to Digest";
            break;
    }
    return oss.str();
}

std::string TryFromHexDigestError::build_message(ErrorType type, const std::string& detail) {
    std::ostringstream oss;
    switch (type) {
        case ErrorType::HexDecode:
            oss << "hex decoding error: " << detail;
            break;
        case ErrorType::Digest:
            oss << "digest error: " << detail;
            break;
    }
    return oss.str();
}

std::string BFieldElementStringConversionError::build_message(ErrorType type, const std::string& detail) {
    std::ostringstream oss;
    switch (type) {
        case ErrorType::Empty:
            oss << "Empty " << (detail.empty() ? "string" : detail) << ".";
            break;
        case ErrorType::InvalidDigit:
            oss << "Invalid digit in string" << (detail.empty() ? "" : ": " + detail);
            break;
        case ErrorType::InvalidHexChar:
            oss << "Invalid hex character" << (detail.empty() ? "" : ": " + detail);
            break;
        case ErrorType::Overflow:
            oss << "Value too large" << (detail.empty() ? "" : ": " + detail);
            break;
        case ErrorType::OutOfRange:
            oss << "Value out of canonical range" << (detail.empty() ? "" : ": " + detail);
            break;
    }
    return oss.str();
}

} // namespace tip5xx

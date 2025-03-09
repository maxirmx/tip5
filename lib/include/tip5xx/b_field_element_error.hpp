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

#include "tip5xx/tip5xx_error.hpp"
#include <sstream>

namespace tip5xx {

class BFieldElement;  // Forward declaration

class BFieldElementError : public Tip5xxError {
public:
    explicit BFieldElementError(const std::string& message) : Tip5xxError(message) {}
};

class BFieldElementInverseError : public BFieldElementError {
public:
    BFieldElementInverseError() : BFieldElementError("Cannot compute multiplicative inverse of zero") {}
};

class BFieldElementPrimitiveRootError : public BFieldElementError {
public:
    BFieldElementPrimitiveRootError() : BFieldElementError("No primitive root of unity exists for this order") {}
};

class BFieldElementStringConversionError : public BFieldElementError {
public:
    enum class ErrorType {
        Empty,
        InvalidDigit,
        InvalidHexChar,
        Overflow,
        OutOfRange
    };

    BFieldElementStringConversionError(ErrorType type, const std::string& detail = "")
        : BFieldElementError(build_message(type, detail)), type_(type) {}

    ErrorType type() const { return type_; }

private:
    ErrorType type_;
    static std::string build_message(ErrorType type, const std::string& detail);
};


// ParseBFieldElementError
class ParseBFieldElementError : public BFieldElementError {
public:
    enum class ErrorType {
        NotCanonical
//        InvalidNumBytes
    };

    ParseBFieldElementError(ErrorType type, const std::string& detail = "")
        : BFieldElementError(build_message(type, detail)), type_(type) {}

    ErrorType type() const { return type_; }

private:
    ErrorType type_;

    static std::string build_message(ErrorType type, const std::string& detail);
};

// TryFromU32sError
class TryFromU32sError : public BFieldElementError {
public:
    TryFromU32sError() : BFieldElementError("U32s<N>: `N` not big enough to hold the value") {}
};


// TryFromXFieldElementError
class TryFromXFieldElementError : public BFieldElementError {
public:
    enum class ErrorType {
        InvalidLength,
        InvalidDigest
    };

    TryFromXFieldElementError(ErrorType type, const std::string& detail = "")
        : BFieldElementError(build_message(type, detail)), type_(type) {}

    ErrorType type() const { return type_; }

private:
    ErrorType type_;

    static std::string build_message(ErrorType type, const std::string& detail);
};

// TryFromDigestError
class TryFromDigestError : public BFieldElementError {
public:
    enum class ErrorType {
        InvalidLength,
        InvalidBFieldElement,
        Overflow
    };

    TryFromDigestError(ErrorType type, const std::string& detail = "")
        : BFieldElementError(build_message(type, detail)), type_(type) {}

    ErrorType type() const { return type_; }

private:
    ErrorType type_;

    static std::string build_message(ErrorType type, const std::string& detail);
};

// TryFromHexDigestError
class TryFromHexDigestError : public BFieldElementError {
public:
    enum class ErrorType {
        HexDecode,
        Digest
    };

    TryFromHexDigestError(ErrorType type, const std::string& detail = "")
        : BFieldElementError(build_message(type, detail)), type_(type) {}

    ErrorType type() const { return type_; }

private:
    ErrorType type_;

    static std::string build_message(ErrorType type, const std::string& detail);
};

} // namespace tip5xx

/**
 *
 * Copyright (c) 2025 Maxim [maxirmx] Samsonov
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * This file is a part of tip5xx library
 *
 */

#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace tip5xx {

class Tip5 {
public:
    Tip5() = default;
    ~Tip5() = default;

    // Prevent copying and moving
    Tip5(const Tip5&) = delete;
    Tip5& operator=(const Tip5&) = delete;
    Tip5(Tip5&&) = delete;
    Tip5& operator=(Tip5&&) = delete;

    // Core functionality
    void process(const std::string& input);
    std::string result() const;

    // Hash a pair of byte arrays
    static std::vector<uint8_t> hash_pair(const std::vector<uint8_t>& left, const std::vector<uint8_t>& right);

    // Hash a variable length sequence of byte arrays
    static std::vector<uint8_t> hash_varlen(const std::vector<std::vector<uint8_t>>& inputs);

private:
    std::string result_;
};

} // namespace tip5xx

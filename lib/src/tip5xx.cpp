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

#include "tip5xx/tip5xx.hpp"

namespace tip5xx {


std::vector<uint8_t> Tip5::hash_pair(const std::vector<uint8_t>& left, const std::vector<uint8_t>& right) {
    // Initialize output vector with fixed size (as in Rust impl)
    std::vector<uint8_t> result(32, 0);

    // TODO: Implement actual TIP5 hashing logic
    // For now returning a placeholder implementation
    for (size_t i = 0; i < std::min(left.size(), result.size()); i++) {
        result[i] = left[i];
    }
    for (size_t i = 0; i < std::min(right.size(), result.size()); i++) {
        result[i] ^= right[i];  // XOR with right input
    }

    return result;
}

std::vector<uint8_t> Tip5::hash_varlen(const std::vector<std::vector<uint8_t>>& inputs) {
    if (inputs.empty()) {
        // Return zero hash for empty input
        return std::vector<uint8_t>(32, 0);
    }

    // Start with first element, ensuring 32-byte size
    std::vector<uint8_t> result(32, 0);
    const auto& first = inputs[0];
    for (size_t i = 0; i < std::min(first.size(), result.size()); i++) {
        result[i] = first[i];
    }

    // Hash each subsequent element with the running result
    for (size_t i = 1; i < inputs.size(); i++) {
        result = hash_pair(result, inputs[i]);
    }

    return result;
}

} // namespace tip5xx

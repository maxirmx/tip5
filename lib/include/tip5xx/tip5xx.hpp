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
#include <array>
#include <cstdint>
#include <cassert>
#include <cstring>
#include <algorithm>
#include "tip5xx/b_field_element.hpp"

namespace tip5xx {

class Digest;

class Tip5 {
public:
    // Hash a pair of digests
    static Digest hash_pair(const Digest& left, const Digest& right);
    // Hash a variable length sequence of byte arrays
    static Digest hash_varlen(const std::vector<BFieldElement>& input);

public:
    // TIP5 parameters
    static constexpr size_t STATE_SIZE = 16;      // Size of the sponge state
    static constexpr size_t RATE = 10;            // Rate (r) of the sponge
    static constexpr size_t CAPACITY = 6;         // Capacity (c) of the sponge
    static constexpr size_t NUM_ROUNDS = 5;       // Number of permutation rounds
    static constexpr size_t NUM_SPLIT_AND_LOOKUP = 4;
    static constexpr size_t LOG2_STATE_SIZE = 4;

private:
    // Helper functions
    static void split_and_lookup(BFieldElement& element);
    static void sbox_layer(std::array<BFieldElement, STATE_SIZE>& state);
    static void mds_generated(std::array<BFieldElement, STATE_SIZE>& state);
    static void round(std::array<BFieldElement, STATE_SIZE>& state, size_t round_idx);
    static void permutation(std::array<BFieldElement, STATE_SIZE>& state);

    // Prevent instantiation, copying and moving as all methods are static
    Tip5() = default;
    ~Tip5() = default;
    Tip5(const Tip5&) = delete;
    Tip5& operator=(const Tip5&) = delete;
    Tip5(Tip5&&) = delete;
    Tip5& operator=(Tip5&&) = delete;
};

} // namespace tip5xx

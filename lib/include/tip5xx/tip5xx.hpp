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
#include <cassert>
#include <cstring>
#include <algorithm>

namespace tip5xx {

class Tip5 {
public:
    // Hash a pair of byte arrays
    static std::vector<uint8_t> hash_pair(const std::vector<uint8_t>& left, const std::vector<uint8_t>& right);

    // Hash a variable length sequence of byte arrays
    static std::vector<uint8_t> hash_varlen(const std::vector<std::vector<uint8_t>>& inputs);

private:
    // TIP5 parameters
    static constexpr size_t STATE_SIZE = 63;  // Size of the sponge state in bytes
    static constexpr size_t RATE = 31;        // Rate (r) of the sponge
    static constexpr size_t CAPACITY = STATE_SIZE - RATE;  // Capacity (c) of the sponge
    static constexpr size_t HASH_SIZE = 32;   // Output hash size in bytes
    static constexpr size_t ROUNDS = 80;      // Number of permutation rounds

    // Round constants table
    static constexpr uint8_t ROUND_CONSTANTS[ROUNDS] = {
        0x01, 0x82, 0x83, 0x04, 0x85, 0x06, 0x07, 0x88,
        0x89, 0x0A, 0x8B, 0x0C, 0x8D, 0x0E, 0x0F, 0x90,
        0x91, 0x12, 0x93, 0x14, 0x95, 0x16, 0x17, 0x98,
        0x99, 0x1A, 0x9B, 0x1C, 0x9D, 0x1E, 0x1F, 0xA0,
        0xA1, 0x22, 0xA3, 0x24, 0xA5, 0x26, 0x27, 0xA8,
        0xA9, 0x2A, 0xAB, 0x2C, 0xAD, 0x2E, 0x2F, 0xB0,
        0xB1, 0x32, 0xB3, 0x34, 0xB5, 0x36, 0x37, 0xB8,
        0xB9, 0x3A, 0xBB, 0x3C, 0xBD, 0x3E, 0x3F, 0xC0,
        0xC1, 0x42, 0xC3, 0x44, 0xC5, 0x46, 0x47, 0xC8,
        0xC9, 0x4A, 0xCB, 0x4C, 0xCD, 0x4E, 0x4F, 0xD0
    };

    // Helper functions
    static void xor_bytes(uint8_t* dest, const uint8_t* src, size_t len);
    static uint8_t rotl8(uint8_t x, unsigned int n);
    static void permute(uint8_t* state);
    static void absorb(uint8_t* state, const uint8_t* data, size_t len);
    static void squeeze(uint8_t* state, uint8_t* output, size_t len);

    // Prevent instantiation, copying and moving as all methods are static
    Tip5() = default;
    ~Tip5() = default;
    Tip5(const Tip5&) = delete;
    Tip5& operator=(const Tip5&) = delete;
    Tip5(Tip5&&) = delete;
    Tip5& operator=(Tip5&&) = delete;

};

} // namespace tip5xx

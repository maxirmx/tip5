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

static constexpr size_t STATE_SIZE = 63;  // Size of the sponge state in bytes
static constexpr size_t RATE = 31;        // Rate (r) of the sponge
static constexpr size_t CAPACITY = STATE_SIZE - RATE;  // Capacity (c) of the sponge
static constexpr size_t HASH_SIZE = 32;   // Output hash size in bytes

namespace {
    // XOR bytes from src into dest
    void xor_bytes(uint8_t* dest, const uint8_t* src, size_t len) {
        for (size_t i = 0; i < len; ++i) {
            dest[i] ^= src[i];
        }
    }

    // TIP5 parameters
    static constexpr size_t ROUNDS = 80;

    // TIP5 round constants table (from Rust implementation)
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

    // TIP5 state manipulation functions
    inline uint8_t rotl8(uint8_t x, unsigned int n) {
        return (x << n) | (x >> (8 - n));
    }

    // TIP5 sponge permutation
    void permute(uint8_t* state) {
        for (size_t round = 0; round < ROUNDS; ++round) {
            // Add round constant to first byte
            state[0] ^= ROUND_CONSTANTS[round];

            // Non-linear layer: Apply S-box to each byte
            for (size_t i = 0; i < STATE_SIZE; ++i) {
                // S-box function: x ← (x + (x ≪ 2)) ⊕ (x ≫ 1)
                uint8_t x = state[i];
                x = x + rotl8(x, 2);
                x = x ^ (x >> 1);
                state[i] = x;
            }

            // Linear mixing layer
            // Mix each byte with its neighbors using rotations and XORs
            std::vector<uint8_t> temp(STATE_SIZE);
            for (size_t i = 0; i < STATE_SIZE; ++i) {
                uint8_t prev = state[(i + STATE_SIZE - 1) % STATE_SIZE];
                uint8_t curr = state[i];
                uint8_t next = state[(i + 1) % STATE_SIZE];

                temp[i] = curr ^ rotl8(prev, 1) ^ rotl8(next, 2);
            }
            std::memcpy(state, temp.data(), STATE_SIZE);
        }
    }

    // Absorb data into the sponge state
    void absorb(uint8_t* state, const uint8_t* data, size_t len) {
        size_t absorbed = 0;
        while (absorbed < len) {
            size_t to_absorb = std::min(RATE, len - absorbed);
            xor_bytes(state, data + absorbed, to_absorb);
            permute(state);
            absorbed += to_absorb;
        }
    }

    // Squeeze output from the sponge state
    void squeeze(uint8_t* state, uint8_t* output, size_t len) {
        size_t squeezed = 0;
        while (squeezed < len) {
            size_t to_squeeze = std::min(RATE, len - squeezed);
            std::memcpy(output + squeezed, state, to_squeeze);
            permute(state);
            squeezed += to_squeeze;
        }
    }
}

std::vector<uint8_t> Tip5::hash_pair(const std::vector<uint8_t>& left, const std::vector<uint8_t>& right) {
    // Initialize state to zero
    std::vector<uint8_t> state(STATE_SIZE, 0);

    // Absorb left input
    absorb(state.data(), left.data(), left.size());

    // Absorb right input
    absorb(state.data(), right.data(), right.size());

    // Squeeze out the hash
    std::vector<uint8_t> result(HASH_SIZE);
    squeeze(state.data(), result.data(), HASH_SIZE);

    return result;
}

std::vector<uint8_t> Tip5::hash_varlen(const std::vector<std::vector<uint8_t>>& inputs) {
    if (inputs.empty()) {
        // Return zero hash for empty input
        return std::vector<uint8_t>(HASH_SIZE, 0);
    }

    // Start with first element
    std::vector<uint8_t> result = hash_pair(inputs[0], inputs[0]);

    // Hash each subsequent element with the running result
    for (size_t i = 1; i < inputs.size(); i++) {
        result = hash_pair(result, inputs[i]);
    }

    return result;
}

} // namespace tip5xx

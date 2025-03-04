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

void Tip5::xor_bytes(uint8_t* dest, const uint8_t* src, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        dest[i] ^= src[i];
    }
}

uint8_t Tip5::rotl8(uint8_t x, unsigned int n) {
    return (x << n) | (x >> (8 - n));
}

void Tip5::permute(uint8_t* state) {
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

void Tip5::absorb(uint8_t* state, const uint8_t* data, size_t len) {
    size_t absorbed = 0;
    while (absorbed < len) {
        size_t to_absorb = std::min(RATE, len - absorbed);
        xor_bytes(state, data + absorbed, to_absorb);
        permute(state);
        absorbed += to_absorb;
    }
}

void Tip5::squeeze(uint8_t* state, uint8_t* output, size_t len) {
    size_t squeezed = 0;
    while (squeezed < len) {
        size_t to_squeeze = std::min(RATE, len - squeezed);
        std::memcpy(output + squeezed, state, to_squeeze);
        permute(state);
        squeezed += to_squeeze;
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

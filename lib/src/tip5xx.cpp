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

#include "tip5xx/tip5xx.hpp"
#include "tip5xx/mds.hpp"

namespace tip5xx {

void Tip5::split_and_lookup(BFieldElement& element) {
    auto bytes = element.raw_bytes();
    for (size_t i = 0; i < 8; i++) {
        bytes[i] = LOOKUP_TABLE[static_cast<size_t>(bytes[i])];
    }
    element = BFieldElement::from_raw_bytes(bytes);
}

void Tip5::sbox_layer() {
    for (size_t i = 0; i < NUM_SPLIT_AND_LOOKUP; i++) {
        split_and_lookup(state[i]);
    }

    for (size_t i = NUM_SPLIT_AND_LOOKUP; i < STATE_SIZE; i++) {
        auto sq = state[i] * state[i];
        auto qu = sq * sq;
        state[i] *= sq * qu;
    }
}

void Tip5::mds_generated() {
    std::array<uint64_t, STATE_SIZE> lo{}, hi{};

    // Split each element into lo and hi limbs
    for (size_t i = 0; i < STATE_SIZE; i++) {
        uint64_t b = state[i].raw_u64();
        lo[i] = b & 0xffffffffUL;
        hi[i] = b >> 32;
    }

    // Process each limb with generated_function
    lo = generated_function(lo);
    hi = generated_function(hi);

    // Combine elementwise as in Rust
    for (size_t i = 0; i < STATE_SIZE; i++) {
        __uint128_t s = (lo[i] >> 4) + (static_cast<__uint128_t>(hi[i]) << 28);
        uint64_t s_hi = static_cast<uint64_t>(s >> 64);
        uint64_t s_lo = static_cast<uint64_t>(s);

        uint64_t res;
        bool over;
        if (__builtin_add_overflow(s_lo, s_hi * 0xffffffffULL, &res)) {
            over = true;
        } else {
            over = false;
        }
        state[i] = BFieldElement::from_raw_u64(over ? res + 0xffffffffULL : res);
    }
}


void Tip5::round(size_t round_index) {
    sbox_layer();
    mds_generated();

    size_t offset = round_index * STATE_SIZE;
    for (size_t i = 0; i < STATE_SIZE; i++) {
        state[i] += bfe_from(ROUND_CONSTANTS_RAW[offset + i]);
    }
}

void Tip5::permutation() {
    for (size_t i = 0; i < NUM_ROUNDS; i++) {
        round(i);
    }
}

std::array<std::array<BFieldElement, STATE_SIZE>, NUM_ROUNDS + 1> Tip5::trace() {
    std::array<std::array<BFieldElement, STATE_SIZE>, NUM_ROUNDS + 1> trace{};
    trace[0] = state;

    for (size_t i = 0; i < NUM_ROUNDS; i++) {
        round(i);
        trace[i + 1] = state;
    }

    return trace;
}

Tip5::Tip5(Domain domain) {
    state.fill(BFieldElement::zero());
    if (domain == Domain::FixedLength) {
        for (size_t i = RATE; i < STATE_SIZE; i++) {
            state[i] = BFieldElement::one();
        }
    }
}

std::array<BFieldElement, Digest::LEN> Tip5::hash_10(const std::array<BFieldElement, RATE>& input) {
    Tip5 sponge(Domain::FixedLength);

    // Absorb input
    for (size_t i = 0; i < RATE; i++) {
        sponge.state[i] = input[i];
    }

    sponge.permutation();

    // Squeeze output
    std::array<BFieldElement, Digest::LEN> result{};
    for (size_t i = 0; i < Digest::LEN; i++) {
        result[i] = sponge.state[i];
    }

    return result;
}

Digest Tip5::hash_pair(const Digest& left, const Digest& right) {
    Tip5 sponge(Domain::FixedLength);

    // Copy left digest values
    for (size_t i = 0; i < Digest::LEN; i++) {
        sponge.state[i] = left.values()[i];
    }

    // Copy right digest values
    for (size_t i = 0; i < Digest::LEN; i++) {
        sponge.state[Digest::LEN + i] = right.values()[i];
    }

    sponge.permutation();

    // Create new digest from first LEN elements
    std::array<BFieldElement, Digest::LEN> result{};
    for (size_t i = 0; i < Digest::LEN; i++) {
        result[i] = sponge.state[i];
    }

    return Digest(result);
}

Digest Tip5::hash_varlen(const std::vector<BFieldElement>& input) {
    Tip5 sponge(Domain::VariableLength);

    // Process input in chunks of RATE size
    size_t pos = 0;
    while (pos + RATE <= input.size()) {
        for (size_t i = 0; i < RATE; i++) {
            sponge.state[i] = input[pos + i];
        }
        sponge.permutation();
        pos += RATE;
    }

    // Handle remaining elements with padding
    size_t remaining = input.size() - pos;
    for (size_t i = 0; i < remaining; i++) {
        sponge.state[i] = input[pos + i];
    }

    // Add padding: 1 followed by 0s
    sponge.state[remaining] = BFieldElement::one();
    for (size_t i = remaining + 1; i < RATE; i++) {
        sponge.state[i] = BFieldElement::zero();
    }

    sponge.permutation();

    // Create digest from first LEN elements
    std::array<BFieldElement, Digest::LEN> result{};
    for (size_t i = 0; i < Digest::LEN; i++) {
        result[i] = sponge.state[i];
    }

    return Digest(result);
}

std::vector<uint32_t> Tip5::sample_indices(uint32_t upper_bound, size_t num_indices) {
    std::vector<uint32_t> indices;
    indices.reserve(num_indices);

    while (indices.size() < num_indices) {
        for (const auto& elem : state) {
            if (elem != BFieldElement::MAX) {
                indices.push_back(static_cast<uint32_t>(elem.value() % upper_bound));
                if (indices.size() == num_indices) break;
            }
        }
        if (indices.size() < num_indices) {
            permutation();
        }
    }

    return indices;
}

void Tip5::absorb(const std::array<BFieldElement, RATE>& input) {
    // Copy input values into the first RATE elements of state
    for (size_t i = 0; i < RATE; ++i) {
        state[i] = input[i];
    }

    // Apply the permutation
    permutation();
}

std::array<BFieldElement, RATE> Tip5::squeeze() {
    // Extract the first RATE elements from the state
    std::array<BFieldElement, RATE> produce;
    std::copy(state.begin(), state.begin() + RATE, produce.begin());

    // Apply the permutation
    permutation();

    return produce;
}


} // namespace tip5xx

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

#include <cmath>
#include "tip5xx/ntt.hpp"

namespace tip5xx {

// Check if slice_len is a power of 2 (n & (n-1) == 0 for powers of 2)
static void ensure_power_of_two(size_t n) {
    if (!n || (n & (n - 1)))
        throw std::invalid_argument("Input length must be a power of 2");
}

template<typename FF>
void ntt(std::vector<FF>& x) {
    const size_t slice_len = x.size();
    if (slice_len == 0) return;
    ensure_power_of_two(slice_len);

    const uint32_t log2_slice_len = static_cast<uint32_t>(std::log2(slice_len));
    const BFieldElement omega = BFieldElement::primitive_root_of_unity(slice_len);
    ntt_unchecked(x, omega, log2_slice_len);
}

template<typename FF>
void intt(std::vector<FF>& x) {
    const size_t slice_len = x.size();
    if (slice_len == 0) return;
    ensure_power_of_two(slice_len);

    const uint32_t log2_slice_len = static_cast<uint32_t>(std::log2(slice_len));
    const BFieldElement omega = BFieldElement::primitive_root_of_unity(slice_len);
    ntt_unchecked(x, omega.inverse(), log2_slice_len);

    // Scale by 1/n
    BFieldElement n_inv = BFieldElement::new_element(static_cast<uint64_t>(slice_len)).inverse();
    for (auto& elem : x) {
        elem *= n_inv;
    }
}

template<typename FF>
void ntt_unchecked(std::vector<FF>& x, const BFieldElement& omega, uint32_t log2_slice_len) {
    const auto slice_len = static_cast<uint32_t>(x.size());

    // Bit reversal permutation
    for (uint32_t k = 0; k < slice_len; k++) {
        const uint32_t rk = bitreverse(k, log2_slice_len);
        if (k < rk) {
            std::swap(x[rk], x[k]);
        }
    }

    // Butterfly operations
    uint32_t m = 1;
    for (uint32_t s = 0; s < log2_slice_len; s++) {
        const BFieldElement w_m = omega.mod_pow_u32(slice_len / (2 * m));
        uint32_t k = 0;
        while (k < slice_len) {
            BFieldElement w = BFieldElement::ONE;
            for (uint32_t j = 0; j < m; j++) {
                FF u = x[k + j];
                FF v = x[k + j + m];
                v *= w;
                x[k + j] = u + v;
                x[k + j + m] = u - v;
                w *= w_m;
            }
            k += 2 * m;
        }
        m *= 2;
    }
}

template<typename FF>
void ntt_noswap(std::vector<FF>& x) {
    const size_t n = x.size();
    if (n == 0) return;
    ensure_power_of_two(n);

    const BFieldElement omega = BFieldElement::primitive_root_of_unity(n);
    const size_t logn = static_cast<size_t>(std::log2(n));

    // Precompute powers of omega in bitreversed order
    std::vector<BFieldElement> powers_of_omega_bitreversed(n, BFieldElement::ZERO);
    BFieldElement omegai = BFieldElement::ONE;
    for (size_t i = 0; i < n / 2; i++) {
        powers_of_omega_bitreversed[bitreverse_usize(i, logn - 1)] = omegai;
        omegai *= omega;
    }

    size_t m = 1;
    size_t t = n;
    while (m < n) {
        t >>= 1;
        for (size_t i = 0; i < m; i++) {
            const size_t s = i * t * 2;
            for (size_t j = s; j < s + t; j++) {
                const FF u = x[j];
                FF v = x[j + t];
                v *= powers_of_omega_bitreversed[i];
                x[j] = u + v;
                x[j + t] = u - v;
            }
        }
        m *= 2;
    }
}

template<typename FF>
void intt_noswap(std::vector<FF>& x) {
    const size_t n = x.size();
    if (n == 0) return;
    ensure_power_of_two(n);

    const BFieldElement omega = BFieldElement::primitive_root_of_unity(n);
    const BFieldElement omega_inverse = omega.inverse();
    const size_t logn = static_cast<size_t>(std::log2(n));

    size_t m = 1;
    for (size_t s = 0; s < logn; s++) {
        const BFieldElement w_m = omega_inverse.mod_pow_u32(static_cast<uint32_t>(n / (2 * m)));
        size_t k = 0;
        while (k < n) {
            BFieldElement w = BFieldElement::ONE;
            for (size_t j = 0; j < m; j++) {
                FF u = x[k + j];
                FF v = x[k + j + m];
                v *= w;
                x[k + j] = u + v;
                x[k + j + m] = u - v;
                w *= w_m;
            }
            k += 2 * m;
        }
        m *= 2;
    }
}

void unscale(std::vector<BFieldElement>& array) {
    if (array.empty()) return;
    const BFieldElement n_inv = BFieldElement::new_element(static_cast<uint64_t>(array.size())).inverse();
    for (auto& elem : array) {
        elem *= n_inv;
    }
}

uint32_t bitreverse(uint32_t n, uint32_t l) {
    uint32_t r = 0;
    for (uint32_t i = 0; i < l; i++) {
        r = (r << 1) | (n & 1);
        n >>= 1;
    }
    return r;
}

size_t bitreverse_usize(size_t n, size_t l) {
    size_t r = 0;
    for (size_t i = 0; i < l; i++) {
        r = (r << 1) | (n & 1);
        n >>= 1;
    }
    return r;
}

template<typename FF>
void bitreverse_order(std::vector<FF>& array) {
    if (array.empty()) return;

    const size_t n = array.size();
    size_t logn = 0;
    while ((size_t(1) << logn) < n) {
        logn++;
    }

    for (size_t k = 0; k < n; k++) {
        const size_t rk = bitreverse_usize(k, logn);
        if (k < rk) {
            std::swap(array[rk], array[k]);
        }
    }
}

// Explicit template instantiations for BFieldElement and XFieldElement
template void ntt<BFieldElement>(std::vector<BFieldElement>&);
template void ntt<XFieldElement>(std::vector<XFieldElement>&);
template void intt<BFieldElement>(std::vector<BFieldElement>&);
template void intt<XFieldElement>(std::vector<XFieldElement>&);
template void ntt_unchecked<BFieldElement>(std::vector<BFieldElement>&, const BFieldElement&, uint32_t);
template void ntt_unchecked<XFieldElement>(std::vector<XFieldElement>&, const BFieldElement&, uint32_t);
template void ntt_noswap<BFieldElement>(std::vector<BFieldElement>&);
template void ntt_noswap<XFieldElement>(std::vector<XFieldElement>&);
template void intt_noswap<BFieldElement>(std::vector<BFieldElement>&);
template void intt_noswap<XFieldElement>(std::vector<XFieldElement>&);
template void bitreverse_order<BFieldElement>(std::vector<BFieldElement>&);
template void bitreverse_order<XFieldElement>(std::vector<XFieldElement>&);

} // namespace tip5xx

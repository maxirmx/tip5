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

#include <vector>
#include <cstdint>
#include <stdexcept>
#include "tip5xx/b_field_element.hpp"
#include "tip5xx/x_field_element.hpp"

namespace tip5xx {

/// Perform Number Theoretic Transform (NTT) on slices of prime-field elements
///
/// NTTs are Number Theoretic Transforms, which are Discrete Fourier Transforms
/// (DFTs) over finite fields. This implementation specifically aims at being
/// used to compute polynomial multiplication over finite fields.
///
/// The transform is performed in-place.
/// If called on an empty array, returns an empty array.
///
/// @tparam FF Finite field element type
/// @param x Array of field elements to transform
/// @throws std::invalid_argument if the length of the input slice is not a power of two
template<typename FF>
void ntt(std::vector<FF>& x);

/// Perform inverse NTT (iNTT) on slices of prime-field elements
///
/// INTT is the inverse NTT, so abstractly, intt(values) = ntt(values) / n
///
/// The transform is performed in-place.
///
/// @tparam FF Finite field element type
/// @param x Array of field elements to transform
/// @throws std::invalid_argument if the length of the input slice is not a power of two
template<typename FF>
void intt(std::vector<FF>& x);

/// Like NTT, but with greater control over the root of unity
/// Does not check whether the root of unity is valid or the log2_slice_len matches
///
/// @param x Array of field elements to transform
/// @param omega Root of unity to use
/// @param log2_slice_len Log base 2 of the slice length
template<typename FF>
void ntt_unchecked(std::vector<FF>& x, const BFieldElement& omega, uint32_t log2_slice_len);

/// Compute the NTT but leave the array in bitreversed order
///
/// This method can be expected to outperform regular NTT when:
/// - it is followed up by INTT (e.g. for fast multiplication)
/// - the powers_of_omega_bitreversed can be precomputed
///
/// @tparam FF Finite field element type
/// @param x Array of field elements to transform
template<typename FF>
void ntt_noswap(std::vector<FF>& x);

/// Compute the inverse NTT, assuming array is in bitreversed order
/// Does not unscale by n afterward.
///
/// @tparam FF Finite field element type
/// @param x Array of field elements to transform
template<typename FF>
void intt_noswap(std::vector<FF>& x);

/// Unscale array by multiplying every element by inverse of array length
///
/// @param array Array to unscale
void unscale(std::vector<BFieldElement>& array);

/// Bit reverse a number n using l bits
///
/// @param n Number to reverse
/// @param l Number of bits
/// @return Bit-reversed number
uint32_t bitreverse(uint32_t n, uint32_t l);

/// Bit reverse a number n using l bits (size_t version)
///
/// @param n Number to reverse
/// @param l Number of bits
/// @return Bit-reversed number
size_t bitreverse_usize(size_t n, size_t l);

/// Reorder array elements according to bit-reversed indices
///
/// @tparam FF Element type
/// @param array Array to reorder
template<typename FF>
void bitreverse_order(std::vector<FF>& array);

} // namespace tip5xx

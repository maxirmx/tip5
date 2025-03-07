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
#include <cassert>
#include <cstdint>

// Base trait for types that can generate cyclic group elements
template <typename Derived>
class CyclicGroupGenerator {
public:
    std::vector<Derived> cyclic_group_elements(size_t max = 0) const {
        return static_cast<const Derived*>(this)->cyclic_group_elements_impl(max);
    }
};

// Base trait for types that have multiplicative inverses
template <typename Derived>
class Inverse {
public:
    Derived inverse() const {
        return static_cast<const Derived*>(this)->inverse_impl();
    }

    Derived inverse_or_zero() const {
        const Derived* derived = static_cast<const Derived*>(this);
        if (derived->is_zero()) {
            return Derived::zero();
        } else {
            return derived->inverse_impl();
        }
    }
};

// Base trait for finding primitive roots of unity
template <typename Derived>
class PrimitiveRootOfUnity {
public:
    static Derived primitive_root_of_unity(uint64_t n) {
        return Derived::primitive_root_of_unity_impl(n);
    }
};

// Base trait for modular exponentiation with u64
template <typename Derived>
class ModPowU64 {
public:
    Derived mod_pow_u64(uint64_t pow) const {
        return static_cast<const Derived*>(this)->mod_pow_u64_impl(pow);
    }
};

// Base trait for modular exponentiation with u32
template <typename Derived>
class ModPowU32 {
public:
    Derived mod_pow_u32(uint32_t exp) const {
        return static_cast<const Derived*>(this)->mod_pow_u32_impl(exp);
    }
};

// Base trait for finite field implementations using CRTP
template <typename Derived>
class FiniteField :
    public CyclicGroupGenerator<Derived>,
    public Inverse<Derived>,
    public PrimitiveRootOfUnity<Derived>,
    public ModPowU64<Derived>,
    public ModPowU32<Derived>
{
public:
    // Static utility for batch inversion
    static std::vector<Derived> batch_inversion(std::vector<Derived> input) {
        size_t input_length = input.size();
        if (input_length == 0) {
            return std::vector<Derived>();
        }

        Derived zero = Derived::zero();
        Derived one = Derived::one();
        std::vector<Derived> scratch(input_length, zero);
        Derived acc = one;

        for (size_t i = 0; i < input_length; i++) {
            assert(!input[i].is_zero() && "Cannot do batch inversion on zero");
            scratch[i] = acc;
            acc *= input[i];
        }

        acc = acc.inverse();

        for (int i = static_cast<int>(input_length) - 1; i >= 0; i--) {
            Derived tmp = acc * input[i];
            input[i] = acc * scratch[i];
            acc = tmp;
        }

        return input;
    }

    // Square helper method
    Derived square() const {
        const Derived* derived = static_cast<const Derived*>(this);
        return (*derived) * (*derived);
    }
};

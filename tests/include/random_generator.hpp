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

#include <random>
#include <vector>
#include "tip5xx/b_field_element.hpp"

using namespace tip5xx;

/**
 * Random generator class for consistent RNG across the library
 */
class RandomGenerator {
private:
    std::mt19937_64 rng;

public:
    // Initialize with random seed
    RandomGenerator() : rng(std::random_device{}()) {}

    // Initialize with specific seed (for testing)
    explicit RandomGenerator(uint64_t seed) : rng(seed) {}

    // Generate random value in range [min, max]
    template<typename T>
    T random_range(T min, T max) {
        if constexpr (std::is_integral_v<T>) {
            std::uniform_int_distribution<T> dist(min, max);
            return dist(rng);
        } else {
            std::uniform_real_distribution<T> dist(min, max);
            return dist(rng);
        }
    }

    template<typename T>
    T random_range(T max) {
        if constexpr (std::is_integral_v<T>) {
            std::uniform_int_distribution<T> dist(static_cast<T>(0), max);
            return dist(rng);
        } else {
            std::uniform_real_distribution<T> dist(static_cast<T>(0), max);
            return dist(rng);
        }
    }

    // Generate random BFieldElement
    BFieldElement random_bfe() {
        std::uniform_int_distribution<uint64_t> dist(0, BFieldElement::MAX);
        return BFieldElement::new_element(dist(rng));
    }

    // Get the underlying engine
    std::mt19937_64& engine() {
        return rng;
    }

    // Generate multiple random BFieldElements
    std::vector<BFieldElement> random_elements(size_t n) {
        std::vector<BFieldElement> elements;
        elements.reserve(n);

        for (size_t i = 0; i < n; i++) {
            elements.push_back(random_bfe());
        }

        return elements;
    }
};

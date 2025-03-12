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

#include "tip5xx/b_field_element.hpp"
#include "tip5xx/zerofier_tree.hpp"
#include "random_generator.hpp"
#include <gtest/gtest.h>
#include <vector>

using namespace tip5xx;

class ZerofierTreeTest : public ::testing::Test {
protected:
    RandomGenerator rng;
};

// Test that empty domain creates a tree with identity polynomial
TEST_F(ZerofierTreeTest, EmptyDomain) {
    std::vector<BFieldElement> domain;
    auto tree = ZerofierTree<BFieldElement>::new_from_domain(domain);
    auto polynomial = tree->zerofier();

    ASSERT_TRUE(polynomial.is_one());
}

// Test single element domain
TEST_F(ZerofierTreeTest, SingleElementDomain) {
    BFieldElement element = rng.random_bfe();
    std::vector<BFieldElement> domain = {element};

    auto tree = ZerofierTree<BFieldElement>::new_from_domain(domain);
    auto polynomial = tree->zerofier();

    ASSERT_EQ(polynomial.degree(), 1);
    ASSERT_EQ(polynomial.evaluate(element), BFieldElement::zero());
}

// Test that zerofier polynomial evaluates to zero at all domain points
TEST_F(ZerofierTreeTest, ZerofierAtDomainPoints) {
    std::vector<BFieldElement> domain;
    for(size_t i = 0; i < 10; i++) {
        domain.push_back(rng.random_bfe());
    }

    auto tree = ZerofierTree<BFieldElement>::new_from_domain(domain);
    auto polynomial = tree->zerofier();

    for(const auto& point : domain) {
        ASSERT_EQ(polynomial.evaluate(point), BFieldElement::zero());
    }
}

// Test that the degree is correct for various sizes
TEST_F(ZerofierTreeTest, DegreeTests) {
    for (size_t size : {1, 15, 16, 17, 31, 32, 33}) {
        std::vector<BFieldElement> domain;
        for(size_t i = 0; i < size; i++) {
            domain.push_back(rng.random_bfe());
        }

        auto tree = ZerofierTree<BFieldElement>::new_from_domain(domain);
        auto polynomial = tree->zerofier();

        ASSERT_EQ(polynomial.degree(), size)
            << "Failed for size " << size;
    }
}

// Test equality operators
TEST_F(ZerofierTreeTest, EqualityOperators) {
    std::vector<BFieldElement> domain;
    for(size_t i = 0; i < 5; i++) {
        domain.push_back(rng.random_bfe());
    }

    auto tree1 = ZerofierTree<BFieldElement>::new_from_domain(domain);
    auto tree2 = ZerofierTree<BFieldElement>::new_from_domain(domain);
    auto tree3 = ZerofierTree<BFieldElement>::new_from_domain({rng.random_bfe()});

    ASSERT_EQ(tree1, tree2);
    ASSERT_NE(tree1, tree3);
}

// Test tree construction with padding
TEST_F(ZerofierTreeTest, PaddedTreeConstruction) {
    // Test sizes that require padding
    for (size_t size : {3, 5, 7}) {
        std::vector<BFieldElement> domain;
        for(size_t i = 0; i < size; i++) {
            domain.push_back(rng.random_bfe());
        }

        auto tree = ZerofierTree<BFieldElement>::new_from_domain(domain);
        auto polynomial = tree->zerofier();

        // Check that padding doesn't affect the result
        ASSERT_EQ(polynomial.degree(), size);
        for(const auto& point : domain) {
            ASSERT_EQ(polynomial.evaluate(point), BFieldElement::zero());
        }
    }
}

// Test with RECURSION_CUTOFF_THRESHOLD sized chunks
TEST_F(ZerofierTreeTest, RecursionCutoffBoundary) {
    // Test around the cutoff threshold
    std::vector<size_t> test_sizes = {15, 16, 17, 31, 32, 33};

    for(size_t size : test_sizes) {
        std::vector<BFieldElement> domain;
        for(size_t i = 0; i < size; i++) {
            domain.push_back(rng.random_bfe());
        }

        auto tree = ZerofierTree<BFieldElement>::new_from_domain(domain);
        auto polynomial = tree->zerofier();

        // Verify correctness regardless of chunking
        ASSERT_EQ(polynomial.degree(), size);
        for(const auto& point : domain) {
            ASSERT_EQ(polynomial.evaluate(point), BFieldElement::zero());
        }
    }
}

// Copyright (c) 2025 Maxim [maxirmx] Samsonov (maxirmx@gmail.com)
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

#pragma once

#include "tip5xx/polynomial.hpp"
#include "tip5xx/b_field_element.hpp"
#include <vector>
#include <memory>

namespace tip5xx {

/**
 * @brief Structure representing a leaf node in the zerofier tree.
 *
 * @tparam FF Finite field type
 */
template<typename FF>
struct Leaf {
    std::vector<FF> points;
    Polynomial<FF> zerofier;

    explicit Leaf(const std::vector<FF>& points);
};

/**
 * @brief Structure representing a branch node in the zerofier tree.
 *
 * @tparam FF Finite field type
 */
template<typename FF>
struct Branch;  // Forward declaration

/**
 * @brief A zerofier tree is a balanced binary tree of vanishing polynomials.
 *
 * Conceptually, every leaf corresponds to a single point, and the value of
 * that leaf is the monic linear polynomial that evaluates to zero there and
 * nowhere else. Every non-leaf node is the product of its two children.
 *
 * In practice, it makes sense to truncate the tree depth, in which case every
 * leaf contains a chunk of points whose size is upper-bounded and more or less
 * equal to some constant threshold.
 *
 * @tparam FF Finite field type
 */
template<typename FF>
class ZerofierTree {
public:
    // Constructors
    ZerofierTree() = default;  // Padding case
    explicit ZerofierTree(const Leaf<FF>& leaf);  // Leaf case
    explicit ZerofierTree(const Branch<FF>& branch);  // Branch case

    // Factory method
    static ZerofierTree new_from_domain(const std::vector<FF>& domain);

    // Core functionality
    Polynomial<FF> zerofier() const;

    // Equality comparison
    bool operator==(const ZerofierTree& other) const;
    bool operator!=(const ZerofierTree& other) const;

private:
    static constexpr size_t RECURSION_CUTOFF_THRESHOLD = 16;

    enum class NodeType {
        Leaf,
        Branch,
        Padding
    };

    NodeType type;
    std::shared_ptr<void> node;  // Either Leaf<FF> or Branch<FF> based on type
};

/**
 * @brief Structure representing a branch node in the zerofier tree.
 *
 * @tparam FF Finite field type
 */
template<typename FF>
struct Branch {
    Polynomial<FF> zerofier;
    ZerofierTree<FF> left;
    ZerofierTree<FF> right;

    Branch(const ZerofierTree<FF>& left, const ZerofierTree<FF>& right);
};

// Explicit instantiations
extern template class ZerofierTree<BFieldElement>;
extern template class ZerofierTree<XFieldElement>;

} // namespace tip5xx

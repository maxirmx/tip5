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

#include "tip5xx/zerofier_tree.hpp"
#include <deque>
#include <algorithm>
#include <cmath>

namespace tip5xx {

// LeafNode implementation
template<typename FF>
LeafNode<FF>::LeafNode(const std::vector<FF>& pts)
    : points(pts),
      zerofier_poly(Polynomial<FF>::zerofier(pts))
{
}

template<typename FF>
bool LeafNode<FF>::equals(const ZerofierNode<FF>* other) const {
    // Try to cast other to LeafNode
    const LeafNode<FF>* leaf_other = dynamic_cast<const LeafNode<FF>*>(other);
    if (!leaf_other) return false;

    // Compare the points
    if (points.size() != leaf_other->points.size()) return false;

    for (size_t i = 0; i < points.size(); i++) {
        if (points[i] != leaf_other->points[i]) return false;
    }

    return true;
}

// BranchNode implementation
template<typename FF>
BranchNode<FF>::BranchNode(std::shared_ptr<ZerofierTree<FF>> l, std::shared_ptr<ZerofierTree<FF>> r)
    : left(std::move(l)),
      right(std::move(r))
{
    // Compute the product of the left and right zerofiers
    zerofier_poly = left->zerofier() * right->zerofier();
}

template<typename FF>
bool BranchNode<FF>::equals(const ZerofierNode<FF>* other) const {
    // Try to cast other to BranchNode
    const BranchNode<FF>* branch_other = dynamic_cast<const BranchNode<FF>*>(other);
    if (!branch_other) return false;

    // Compare the left and right children
    return *left == *(branch_other->left) && *right == *(branch_other->right);
}

// ZerofierTree implementation
template<typename FF>
ZerofierTree<FF>::ZerofierTree(const std::vector<FF>& points) {
    if (points.empty()) {
        // Handle empty points as a default/empty tree
        node = nullptr;
    } else {
        // Create a leaf node
        node = std::make_shared<LeafNode<FF>>(points);
    }
}

template<typename FF>
ZerofierTree<FF>::ZerofierTree(std::shared_ptr<ZerofierTree<FF>> left, std::shared_ptr<ZerofierTree<FF>> right) {
    if (!left && !right) {
        // Both children are null, create empty tree
        node = nullptr;
    } else if (!left) {
        // Left child is null, use right child
        node = right->node;
    } else if (!right) {
        // Right child is null, use left child
        node = left->node;
    } else {
        // Both children exist, create a branch
        node = std::make_shared<BranchNode<FF>>(std::move(left), std::move(right));
    }
}

template<typename FF>
bool ZerofierTree<FF>::operator==(const ZerofierTree& other) const {
    // Handle null cases
    if (!node && !other.node) return true;
    if (!node || !other.node) return false;

    // Delegate to the node's equals method
    return node->equals(other.node.get());
}

template<typename FF>
std::shared_ptr<ZerofierTree<FF>> ZerofierTree<FF>::new_from_domain(const std::vector<FF>& domain) {
    static constexpr size_t RECURSION_CUTOFF_THRESHOLD = 16;

    if (domain.empty()) {
        // Empty domain case
        return std::make_shared<ZerofierTree<FF>>();
    }

    // Calculate number of leaf nodes needed
    size_t num_points = domain.size();
    size_t leaf_size = std::min(RECURSION_CUTOFF_THRESHOLD, num_points);
    size_t num_leaves = (num_points + leaf_size - 1) / leaf_size; // Ceiling division

    // Round up to the next power of 2
    size_t padded_leaves = 1;
    while (padded_leaves < num_leaves) {
        padded_leaves *= 2;
    }

    // Create a queue of trees
    std::deque<std::shared_ptr<ZerofierTree<FF>>> trees;

    // Create leaf nodes
    for (size_t i = 0; i < num_points; i += leaf_size) {
        size_t end_idx = std::min(i + leaf_size, num_points);
        std::vector<FF> leaf_points(domain.begin() + i, domain.begin() + end_idx);

        trees.push_back(std::make_shared<ZerofierTree<FF>>(leaf_points));
    }

    // Add padding nodes if needed
    while (trees.size() < padded_leaves) {
        trees.push_back(std::make_shared<ZerofierTree<FF>>());
    }

    // Build the tree from bottom-up
    while (trees.size() > 1) {
        auto left = trees.front();
        trees.pop_front();

        auto right = trees.front();
        trees.pop_front();

        trees.push_back(std::make_shared<ZerofierTree<FF>>(left, right));
    }

    // Return the root
    return trees.front();
}

// Explicit template instantiations
template class LeafNode<BFieldElement>;
template class LeafNode<XFieldElement>;
template class BranchNode<BFieldElement>;
template class BranchNode<XFieldElement>;
template class ZerofierTree<BFieldElement>;
template class ZerofierTree<XFieldElement>;

} // namespace tip5xx

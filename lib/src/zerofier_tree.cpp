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

namespace tip5xx {

template<typename FF>
Leaf<FF>::Leaf(const std::vector<FF>& points)
    : points(points)
    , zerofier(Polynomial<FF>::zerofier(points))
{}

template<typename FF>
Branch<FF>::Branch(const ZerofierTree<FF>& left, const ZerofierTree<FF>& right)
    : left(left)
    , right(right)
    , zerofier(left.zerofier() * right.zerofier())
{}

template<typename FF>
ZerofierTree<FF>::ZerofierTree(const Leaf<FF>& leaf)
    : type(NodeType::Leaf)
    , node(std::make_shared<Leaf<FF>>(leaf))
{}

template<typename FF>
ZerofierTree<FF>::ZerofierTree(const Branch<FF>& branch)
    : type(NodeType::Branch)
    , node(std::make_shared<Branch<FF>>(branch))
{}

template<typename FF>
ZerofierTree<FF> ZerofierTree<FF>::new_from_domain(const std::vector<FF>& domain) {
    std::deque<ZerofierTree<FF>> nodes;

    // Create leaf nodes
    for (size_t i = 0; i < domain.size(); i += RECURSION_CUTOFF_THRESHOLD) {
        size_t chunk_size = std::min(RECURSION_CUTOFF_THRESHOLD, domain.size() - i);
        std::vector<FF> chunk(domain.begin() + i, domain.begin() + i + chunk_size);
        nodes.push_back(ZerofierTree(Leaf<FF>(chunk)));
    }

    // Pad to power of 2
    size_t padded_size = nodes.size();
    if (padded_size & (padded_size - 1)) { // If not power of 2
        padded_size = 1 << (64 - __builtin_clzl(padded_size));
    }
    while (nodes.size() < padded_size) {
        nodes.push_back(ZerofierTree<FF>());  // Padding node
    }

    // Build tree bottom-up
    while (nodes.size() > 1) {
        auto right = nodes.back();
        nodes.pop_back();
        auto left = nodes.back();
        nodes.pop_back();

        if (left.type == NodeType::Padding) {
            nodes.push_front(ZerofierTree<FF>());
        } else {
            nodes.push_front(ZerofierTree(Branch<FF>(left, right)));
        }
    }

    return nodes.front();
}

template<typename FF>
Polynomial<FF> ZerofierTree<FF>::zerofier() const {
    switch (type) {
        case NodeType::Leaf:
            return std::static_pointer_cast<Leaf<FF>>(node)->zerofier;
        case NodeType::Branch:
            return std::static_pointer_cast<Branch<FF>>(node)->zerofier;
        default:  // Padding
            return Polynomial<FF>::one();
    }
}

template<typename FF>
bool ZerofierTree<FF>::operator==(const ZerofierTree& other) const {
    if (type != other.type) return false;
    if (type == NodeType::Padding) return true;

    switch (type) {
        case NodeType::Leaf: {
            auto this_leaf = std::static_pointer_cast<Leaf<FF>>(node);
            auto other_leaf = std::static_pointer_cast<Leaf<FF>>(other.node);
            return this_leaf->points == other_leaf->points &&
                   this_leaf->zerofier == other_leaf->zerofier;
        }
        case NodeType::Branch: {
            auto this_branch = std::static_pointer_cast<Branch<FF>>(node);
            auto other_branch = std::static_pointer_cast<Branch<FF>>(other.node);
            return this_branch->zerofier == other_branch->zerofier &&
                   this_branch->left == other_branch->left &&
                   this_branch->right == other_branch->right;
        }
        default:
            return true;  // Both padding
    }
}

template<typename FF>
bool ZerofierTree<FF>::operator!=(const ZerofierTree& other) const {
    return !(*this == other);
}

// Explicit template instantiations
template class ZerofierTree<BFieldElement>;
template class ZerofierTree<XFieldElement>;

} // namespace tip5xx

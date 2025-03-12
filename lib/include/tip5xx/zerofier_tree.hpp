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

// Forward declarations
template<typename FF> class ZerofierTree;

template<typename FF>
class ZerofierNode {
public:
    virtual ~ZerofierNode() = default;
    virtual Polynomial<FF> zerofier() const = 0;
    virtual bool equals(const ZerofierNode<FF>* other) const = 0;
};

template<typename FF>
class LeafNode : public ZerofierNode<FF> {
public:
    std::vector<FF> points;
    Polynomial<FF> zerofier_poly;

    explicit LeafNode(const std::vector<FF>& pts);
    Polynomial<FF> zerofier() const override { return zerofier_poly; }
    bool equals(const ZerofierNode<FF>* other) const override;
};

template<typename FF>
class BranchNode : public ZerofierNode<FF> {
public:
    Polynomial<FF> zerofier_poly;
    std::shared_ptr<ZerofierTree<FF>> left;
    std::shared_ptr<ZerofierTree<FF>> right;

    BranchNode(std::shared_ptr<ZerofierTree<FF>> l, std::shared_ptr<ZerofierTree<FF>> r);
    Polynomial<FF> zerofier() const override { return zerofier_poly; }
    bool equals(const ZerofierNode<FF>* other) const override;
};

template<typename FF>
class ZerofierTree {
public:
    ZerofierTree() = default;  // Empty/padding case
    explicit ZerofierTree(const std::vector<FF>& points); // Leaf case
    ZerofierTree(std::shared_ptr<ZerofierTree<FF>> left, std::shared_ptr<ZerofierTree<FF>> right); // Branch case

    // Factory method
    static std::shared_ptr<ZerofierTree<FF>> new_from_domain(const std::vector<FF>& domain);

    // Core functionality
    Polynomial<FF> zerofier() const { return node ? node->zerofier() : Polynomial<FF>::one(); }

    // Equality comparison
    bool operator==(const ZerofierTree& other) const;
    bool operator!=(const ZerofierTree& other) const { return !(*this == other); };

private:
    std::shared_ptr<ZerofierNode<FF>> node;
};

template<typename FF>
bool operator==(const std::shared_ptr<ZerofierTree<FF>>& lhs,
                const std::shared_ptr<ZerofierTree<FF>>& rhs) {
    // Handle null cases
    if (!lhs && !rhs) return true;
    if (!lhs || !rhs) return false;

    // Delegate to the ZerofierTree's equality operator
    return *lhs == *rhs;
}

template<typename FF>
bool operator!=(const std::shared_ptr<ZerofierTree<FF>>& lhs,
                const std::shared_ptr<ZerofierTree<FF>>& rhs) {
    return !(lhs == rhs);
}

} // namespace tip5xx

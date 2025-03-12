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

    ASSERT_EQ(*tree1, *tree2);
    ASSERT_NE(*tree1, *tree3);

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

// Test fixture specifically for testing LeafNode
class LeafNodeTest : public ::testing::Test {
    protected:
        RandomGenerator rng;
};

// Test LeafNode equality comparison with different node types
TEST_F(LeafNodeTest, EqualsMethodWithDifferentTypes) {
    // Create a leaf node
    std::vector<BFieldElement> points = {
        BFieldElement::new_element(1),
        BFieldElement::new_element(2),
        BFieldElement::new_element(3)
    };
    LeafNode<BFieldElement> leaf_node(points);

    // Create a branch node (different type)
    auto tree1 = std::make_shared<ZerofierTree<BFieldElement>>(
        std::vector<BFieldElement>{BFieldElement::new_element(4)});
    auto tree2 = std::make_shared<ZerofierTree<BFieldElement>>(
        std::vector<BFieldElement>{BFieldElement::new_element(5)});
    BranchNode<BFieldElement> branch_node(tree1, tree2);

    // Test comparison with different node type
    ASSERT_FALSE(leaf_node.equals(&branch_node));

    // Create another leaf but with different number of points
    std::vector<BFieldElement> different_size_points = {
        BFieldElement::new_element(1),
        BFieldElement::new_element(2)
    };
    LeafNode<BFieldElement> different_size_leaf(different_size_points);

    // Test comparison with different size leaf
    ASSERT_FALSE(leaf_node.equals(&different_size_leaf));

    // Create another leaf with same size but different points
    std::vector<BFieldElement> different_points = {
        BFieldElement::new_element(1),
        BFieldElement::new_element(2),
        BFieldElement::new_element(4) // Different from original
    };
    LeafNode<BFieldElement> different_points_leaf(different_points);

    // Test comparison with same size but different points
    ASSERT_FALSE(leaf_node.equals(&different_points_leaf));

    // Create an identical leaf
    LeafNode<BFieldElement> identical_leaf(points);

    // Test comparison with identical leaf
    ASSERT_TRUE(leaf_node.equals(&identical_leaf));
}

// Test LeafNode equality with null and self
TEST_F(LeafNodeTest, EqualsMethodNullAndSelf) {
    // Create a leaf node
    std::vector<BFieldElement> points = {
        BFieldElement::new_element(1),
        BFieldElement::new_element(2)
    };
    LeafNode<BFieldElement> leaf_node(points);

    // Test comparison with null (should be handled by dynamic_cast)
    ZerofierNode<BFieldElement>* null_node = nullptr;
    ASSERT_FALSE(leaf_node.equals(null_node));

    // Test comparison with self (should be true)
    ASSERT_TRUE(leaf_node.equals(&leaf_node));
}

// Test LeafNode equality with boundary cases
TEST_F(LeafNodeTest, EqualsMethodBoundaryCases) {
    // Create an empty leaf node
    std::vector<BFieldElement> empty_points;
    LeafNode<BFieldElement> empty_leaf(empty_points);

    // Another empty leaf node should be equal
    LeafNode<BFieldElement> another_empty_leaf(empty_points);
    ASSERT_TRUE(empty_leaf.equals(&another_empty_leaf));

    // Create a leaf with special values
    std::vector<BFieldElement> special_points = {
        BFieldElement::ZERO,
        BFieldElement::ONE,
        BFieldElement::MAX
    };
    LeafNode<BFieldElement> special_leaf(special_points);

    // Create a leaf with the same special values but in different order
    std::vector<BFieldElement> reordered_special_points = {
        BFieldElement::ONE,
        BFieldElement::MAX,
        BFieldElement::ZERO
    };
    LeafNode<BFieldElement> reordered_special_leaf(reordered_special_points);

    // Test comparison with reordered points (should be different)
    ASSERT_FALSE(special_leaf.equals(&reordered_special_leaf));
}

// Test LeafNode across different field types
TEST_F(LeafNodeTest, EqualsMethodAcrossFieldTypes) {
    // Test with XFieldElement
    std::vector<XFieldElement> x_points = {
        XFieldElement::new_const(BFieldElement::new_element(1)),
        XFieldElement::new_const(BFieldElement::new_element(2))
    };
    LeafNode<XFieldElement> x_leaf(x_points);

    // Create an identical leaf
    LeafNode<XFieldElement> identical_x_leaf(x_points);

    // Test comparison
    ASSERT_TRUE(x_leaf.equals(&identical_x_leaf));

    // Create a different leaf
    std::vector<XFieldElement> different_x_points = {
        XFieldElement::new_const(BFieldElement::new_element(3)),
        XFieldElement::new_const(BFieldElement::new_element(4))
    };
    LeafNode<XFieldElement> different_x_leaf(different_x_points);

    // Test comparison
    ASSERT_FALSE(x_leaf.equals(&different_x_leaf));
}

// Test LeafNode with random data to ensure coverage
TEST_F(LeafNodeTest, EqualsMethodRandomizedTest) {
    // Create multiple random leaf nodes and compare them
    for (int i = 0; i < 5; i++) {
        // Generate random points
        std::vector<BFieldElement> random_points1;
        std::vector<BFieldElement> random_points2;

        // Generate 5 random points
        for (int j = 0; j < 5; j++) {
            BFieldElement point = rng.random_bfe();
            random_points1.push_back(point);

            // For second set, 50% chance to be different
            if (rand() % 2 == 0) {
                random_points2.push_back(point);
            } else {
                random_points2.push_back(rng.random_bfe());
            }
        }

        LeafNode<BFieldElement> leaf1(random_points1);
        LeafNode<BFieldElement> leaf2(random_points2);

        // Check if points are actually the same
        bool should_be_equal = true;
        if (random_points1.size() != random_points2.size()) {
            should_be_equal = false;
        } else {
            for (size_t j = 0; j < random_points1.size(); j++) {
                if (random_points1[j] != random_points2[j]) {
                    should_be_equal = false;
                    break;
                }
            }
        }

        // Test the equals method
        ASSERT_EQ(should_be_equal, leaf1.equals(&leaf2));
    }
}

// Test fixture specifically for testing BranchNode
class BranchNodeTest : public ::testing::Test {
    protected:
        RandomGenerator rng;

        // Helper method to create a simple tree
        std::shared_ptr<ZerofierTree<BFieldElement>> createSimpleTree(BFieldElement value) {
            return std::make_shared<ZerofierTree<BFieldElement>>(
                std::vector<BFieldElement>{value});
        }
};

// Test BranchNode equality comparison with different node types
TEST_F(BranchNodeTest, EqualsMethodWithDifferentTypes) {
    // Create a branch node
    auto left_tree = createSimpleTree(BFieldElement::new_element(1));
    auto right_tree = createSimpleTree(BFieldElement::new_element(2));
    BranchNode<BFieldElement> branch_node(left_tree, right_tree);

    // Create a leaf node (different type)
    std::vector<BFieldElement> points = {
        BFieldElement::new_element(1),
        BFieldElement::new_element(2)
    };
    LeafNode<BFieldElement> leaf_node(points);

    // Test comparison with different node type
    ASSERT_FALSE(branch_node.equals(&leaf_node));

    // Create an identical branch node
    BranchNode<BFieldElement> identical_branch(left_tree, right_tree);

    // Test comparison with identical branch
    ASSERT_TRUE(branch_node.equals(&identical_branch));
}

// Test BranchNode equality with null and self
TEST_F(BranchNodeTest, EqualsMethodNullAndSelf) {
    // Create a branch node
    auto left_tree = createSimpleTree(BFieldElement::new_element(1));
    auto right_tree = createSimpleTree(BFieldElement::new_element(2));
    BranchNode<BFieldElement> branch_node(left_tree, right_tree);

    // Test comparison with null (should be handled by dynamic_cast)
    ZerofierNode<BFieldElement>* null_node = nullptr;
    ASSERT_FALSE(branch_node.equals(null_node));

    // Test comparison with self (should be true)
    ASSERT_TRUE(branch_node.equals(&branch_node));
}

// Test BranchNode equality with different subtrees
TEST_F(BranchNodeTest, EqualsMethodDifferentSubtrees) {
    // Create two different branch nodes with the same left subtree but different right subtree
    auto common_left = createSimpleTree(BFieldElement::new_element(1));
    auto right1 = createSimpleTree(BFieldElement::new_element(2));
    auto right2 = createSimpleTree(BFieldElement::new_element(3)); // Different

    BranchNode<BFieldElement> branch1(common_left, right1);
    BranchNode<BFieldElement> branch2(common_left, right2);

    // Test comparison (should be false due to different right subtrees)
    ASSERT_FALSE(branch1.equals(&branch2));

    // Create two different branch nodes with the same right subtree but different left subtree
    auto common_right = createSimpleTree(BFieldElement::new_element(4));
    auto left1 = createSimpleTree(BFieldElement::new_element(5));
    auto left2 = createSimpleTree(BFieldElement::new_element(6)); // Different

    BranchNode<BFieldElement> branch3(left1, common_right);
    BranchNode<BFieldElement> branch4(left2, common_right);

    // Test comparison (should be false due to different left subtrees)
    ASSERT_FALSE(branch3.equals(&branch4));
}

// Test BranchNode equality with multi-level trees
TEST_F(BranchNodeTest, EqualsMethodMultiLevel) {
    // Create first level of leaves
    auto leaf1 = createSimpleTree(BFieldElement::new_element(1));
    auto leaf2 = createSimpleTree(BFieldElement::new_element(2));
    auto leaf3 = createSimpleTree(BFieldElement::new_element(3));
    auto leaf4 = createSimpleTree(BFieldElement::new_element(4));

    // Create second level branches
    auto branch_level1_a = std::make_shared<ZerofierTree<BFieldElement>>(leaf1, leaf2);
    auto branch_level1_b = std::make_shared<ZerofierTree<BFieldElement>>(leaf3, leaf4);
    auto branch_level1_c = std::make_shared<ZerofierTree<BFieldElement>>(leaf1, leaf3); // Different combination

    // Create top-level branches
    BranchNode<BFieldElement> top_branch1(branch_level1_a, branch_level1_b);
    BranchNode<BFieldElement> top_branch2(branch_level1_a, branch_level1_b); // Identical to top_branch1
    BranchNode<BFieldElement> top_branch3(branch_level1_a, branch_level1_c); // Different

    // Test comparison of identical multi-level branches
    ASSERT_TRUE(top_branch1.equals(&top_branch2));

    // Test comparison of different multi-level branches
    ASSERT_FALSE(top_branch1.equals(&top_branch3));
}

// Test BranchNode equality with XFieldElement
TEST_F(BranchNodeTest, EqualsMethodXFieldElement) {
    // Create XFieldElement trees
    auto leaf1 = std::make_shared<ZerofierTree<XFieldElement>>(
        std::vector<XFieldElement>{XFieldElement::new_const(BFieldElement::new_element(1))});
    auto leaf2 = std::make_shared<ZerofierTree<XFieldElement>>(
        std::vector<XFieldElement>{XFieldElement::new_const(BFieldElement::new_element(2))});
    auto leaf3 = std::make_shared<ZerofierTree<XFieldElement>>(
        std::vector<XFieldElement>{XFieldElement::new_const(BFieldElement::new_element(3))});

    // Create branch nodes
    BranchNode<XFieldElement> branch1(leaf1, leaf2);
    BranchNode<XFieldElement> branch2(leaf1, leaf2); // Identical to branch1
    BranchNode<XFieldElement> branch3(leaf1, leaf3); // Different

    // Test comparison of identical branches
    ASSERT_TRUE(branch1.equals(&branch2));

    // Test comparison of different branches
    ASSERT_FALSE(branch1.equals(&branch3));
}

// Test BranchNode with shared subtrees
TEST_F(BranchNodeTest, EqualsMethodSharedSubtrees) {
    // Create a common subtree
    auto common_subtree = createSimpleTree(BFieldElement::new_element(1));

    // Create branches that share the same subtree for both left and right
    BranchNode<BFieldElement> branch1(common_subtree, common_subtree);
    BranchNode<BFieldElement> branch2(common_subtree, common_subtree);

    // Test comparison (should be true)
    ASSERT_TRUE(branch1.equals(&branch2));

    // Create a different subtree
    auto different_subtree = createSimpleTree(BFieldElement::new_element(2));

    // Create a branch with the common subtree on left but different on right
    BranchNode<BFieldElement> branch3(common_subtree, different_subtree);

    // Test comparison (should be false)
    ASSERT_FALSE(branch1.equals(&branch3));
}

// Test BranchNode with complex structures
TEST_F(BranchNodeTest, EqualsMethodComplexStructures) {
    // Create domains for testing
    std::vector<BFieldElement> domain1 = {
        BFieldElement::new_element(1),
        BFieldElement::new_element(2),
        BFieldElement::new_element(3)
    };

    std::vector<BFieldElement> domain2 = {
        BFieldElement::new_element(4),
        BFieldElement::new_element(5),
        BFieldElement::new_element(6)
    };

    // Create complex trees using the factory method
    auto tree1 = ZerofierTree<BFieldElement>::new_from_domain(domain1);
    auto tree2 = ZerofierTree<BFieldElement>::new_from_domain(domain1); // Same as tree1
    auto tree3 = ZerofierTree<BFieldElement>::new_from_domain(domain2); // Different

    // The factory creates a tree with internal branches - get the root branch
    // This requires more complex setup, assuming the factory creates a balanced tree
    auto domain_combined = domain1;
    domain_combined.insert(domain_combined.end(), domain2.begin(), domain2.end());
    auto combined_tree1 = ZerofierTree<BFieldElement>::new_from_domain(domain_combined);
    auto combined_tree2 = ZerofierTree<BFieldElement>::new_from_domain(domain_combined); // Same structure

    // Extract and compare the actual branch nodes
    // Note: This is a bit of a hack since we're accessing private members
    // This test might need adjustment based on the actual implementation
    const ZerofierNode<BFieldElement>* node1 = reinterpret_cast<const BranchNode<BFieldElement>*>(
        combined_tree1.get());
    const ZerofierNode<BFieldElement>* node2 = reinterpret_cast<const BranchNode<BFieldElement>*>(
        combined_tree2.get());

    // If the extraction above is not possible, an alternative is to create BranchNodes directly
    BranchNode<BFieldElement> explicit_branch1(tree1, tree3);
    BranchNode<BFieldElement> explicit_branch2(tree1, tree3); // Same
    BranchNode<BFieldElement> explicit_branch3(tree2, tree3); // Same (tree2 = tree1)
    BranchNode<BFieldElement> explicit_branch4(tree3, tree1); // Different (swapped)

    ASSERT_TRUE(explicit_branch1.equals(&explicit_branch2));
    ASSERT_TRUE(explicit_branch1.equals(&explicit_branch3));
    ASSERT_FALSE(explicit_branch1.equals(&explicit_branch4));
}

// Test BranchNode with randomized trees
TEST_F(BranchNodeTest, EqualsMethodRandomized) {
    for (int i = 0; i < 5; i++) {
        // Generate random domains
        std::vector<BFieldElement> domain1;
        std::vector<BFieldElement> domain2;

        // Random domain size between 1-5
        size_t size1 = 1 + (rand() % 5);
        size_t size2 = 1 + (rand() % 5);

        for (size_t j = 0; j < size1; j++) {
            domain1.push_back(rng.random_bfe());
        }

        // 50% chance of having the same domain
        bool same_domain = (rand() % 2) == 0;
        if (same_domain) {
            domain2 = domain1;
        } else {
            for (size_t j = 0; j < size2; j++) {
                domain2.push_back(rng.random_bfe());
            }
        }

        // Create trees
        auto left1 = ZerofierTree<BFieldElement>::new_from_domain(domain1);
        auto right1 = ZerofierTree<BFieldElement>::new_from_domain(domain2);

        auto left2 = same_domain ?
            ZerofierTree<BFieldElement>::new_from_domain(domain1) :
            ZerofierTree<BFieldElement>::new_from_domain(std::vector<BFieldElement>{rng.random_bfe()});

        auto right2 = same_domain ?
            ZerofierTree<BFieldElement>::new_from_domain(domain2) :
            ZerofierTree<BFieldElement>::new_from_domain(std::vector<BFieldElement>{rng.random_bfe()});

        // Create branch nodes
        BranchNode<BFieldElement> branch1(left1, right1);
        BranchNode<BFieldElement> branch2(left2, right2);

        // Should be equal only if using the same domains
        ASSERT_EQ(same_domain, branch1.equals(&branch2));
    }
}

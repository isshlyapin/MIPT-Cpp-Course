#include <string>

#include <gtest/gtest.h>

#include "tree.hpp"

using namespace myds;

TEST(ThreadedBinaryTree, InsertFindSimple) {
    ThreadedBinaryTree<int, std::string> tree;
    EXPECT_TRUE(tree.insert(5, "five"));
    EXPECT_TRUE(tree.insert(3, "three"));
    EXPECT_TRUE(tree.insert(7, "seven"));
    EXPECT_FALSE(tree.insert(5, "five again"));
    auto* n5 = tree.find(5);
    ASSERT_NE(n5, nullptr);
    EXPECT_EQ(n5->value, "five");
    auto* n3 = tree.find(3);
    ASSERT_NE(n3, nullptr);
    EXPECT_EQ(n3->value, "three");
    auto* n7 = tree.find(7);
    ASSERT_NE(n7, nullptr);
    EXPECT_EQ(n7->value, "seven");
    EXPECT_EQ(tree.find(42), nullptr);
}

TEST(ThreadedBinaryTree, RemoveSimple) {
    ThreadedBinaryTree<int, std::string> tree;
    tree.insert(5, "five");
    tree.insert(3, "three");
    tree.insert(7, "seven");
    EXPECT_TRUE(tree.remove(3));
    EXPECT_EQ(tree.find(3), nullptr);
    EXPECT_TRUE(tree.remove(5));
    EXPECT_EQ(tree.find(5), nullptr);
    EXPECT_TRUE(tree.remove(7));
    EXPECT_EQ(tree.find(7), nullptr);
    EXPECT_FALSE(tree.remove(42));
}

TEST(ThreadedBinaryTree, InorderTraversal) {
    ThreadedBinaryTree<int, std::string> tree;
    tree.insert(5, "five");
    tree.insert(3, "three");
    tree.insert(7, "seven");
    std::vector<int> keys;
    tree.inorder([&](auto* node) {
        keys.push_back(node->key);
    });
    EXPECT_EQ(keys, (std::vector<int>{3, 5, 7}));
}

TEST(ThreadedBinaryTree, CopyAndAssign) {
    ThreadedBinaryTree<int, std::string> tree;
    tree.insert(1, "one");
    tree.insert(2, "two");
    tree.insert(3, "three");
    ThreadedBinaryTree<int, std::string> copy(tree);
    EXPECT_NE(copy.find(1), nullptr);
    EXPECT_NE(copy.find(2), nullptr);
    EXPECT_NE(copy.find(3), nullptr);
    ThreadedBinaryTree<int, std::string> assigned;
    assigned = tree;
    EXPECT_NE(assigned.find(1), nullptr);
    EXPECT_NE(assigned.find(2), nullptr);
    EXPECT_NE(assigned.find(3), nullptr);
}

TEST(ThreadedBinaryTree, LowerBound) {
    ThreadedBinaryTree<int, std::string> tree;
    tree.insert(10, "ten");
    tree.insert(5, "five");
    tree.insert(15, "fifteen");
    tree.insert(3, "three");
    tree.insert(7, "seven");
    
    auto* lb3 = tree.lower_bound(3);
    ASSERT_NE(lb3, nullptr);
    EXPECT_EQ(lb3->key, 3);
    
    auto* lb4 = tree.lower_bound(4);
    ASSERT_NE(lb4, nullptr);
    EXPECT_EQ(lb4->key, 5);
    
    auto* lb6 = tree.lower_bound(6);
    ASSERT_NE(lb6, nullptr);
    EXPECT_EQ(lb6->key, 7);
    
    auto* lb20 = tree.lower_bound(20);
    EXPECT_EQ(lb20, nullptr);
}

TEST(ThreadedBinaryTree, UpperBound) {
    ThreadedBinaryTree<int, std::string> tree;
    tree.insert(10, "ten");
    tree.insert(5, "five");
    tree.insert(15, "fifteen");
    tree.insert(3, "three");
    tree.insert(7, "seven");
    
    auto* ub3 = tree.upper_bound(3);
    ASSERT_NE(ub3, nullptr);
    EXPECT_EQ(ub3->key, 5);
    
    auto* ub5 = tree.upper_bound(5);
    ASSERT_NE(ub5, nullptr);
    EXPECT_EQ(ub5->key, 7);
    
    auto* ub14 = tree.upper_bound(14);
    ASSERT_NE(ub14, nullptr);
    EXPECT_EQ(ub14->key, 15);
    
    auto* ub20 = tree.upper_bound(20);
    EXPECT_EQ(ub20, nullptr);
}

TEST(ThreadedBinaryTree, ConstLowerBound) {
    ThreadedBinaryTree<int, std::string> tree;
    tree.insert(10, "ten");
    tree.insert(5, "five");
    tree.insert(15, "fifteen");
    
    const auto& ctree = tree;
    auto* lb5 = ctree.lower_bound(5);
    ASSERT_NE(lb5, nullptr);
    EXPECT_EQ(lb5->key, 5);
}

TEST(ThreadedBinaryTree, ConstUpperBound) {
    ThreadedBinaryTree<int, std::string> tree;
    tree.insert(10, "ten");
    tree.insert(5, "five");
    tree.insert(15, "fifteen");
    
    const auto& ctree = tree;
    auto* ub5 = ctree.upper_bound(5);
    ASSERT_NE(ub5, nullptr);
    EXPECT_EQ(ub5->key, 10);
}

TEST(ThreadedBinaryTree, DistanceSameNode) {
    ThreadedBinaryTree<int, std::string> tree;
    tree.insert(5, "five");
    auto* n5 = tree.find(5);
    EXPECT_EQ(tree.distance(n5, n5), 0);
}

TEST(ThreadedBinaryTree, DistanceToEnd) {
    ThreadedBinaryTree<int, std::string> tree;
    tree.insert(5, "five");
    tree.insert(3, "three");
    tree.insert(7, "seven");
    
    auto* n3 = tree.find(3);
    auto* n5 = tree.find(5);
    auto* n7 = tree.find(7);
    
    // Distance from node to end (nullptr)
    EXPECT_EQ(tree.distance(n7, tree.end()), 1);
    EXPECT_EQ(tree.distance(n3, tree.end()), 3);
    EXPECT_EQ(tree.distance(n5, tree.end()), 2);
}

TEST(ThreadedBinaryTree, DistanceFromEnd) {
    ThreadedBinaryTree<int, std::string> tree;
    tree.insert(5, "five");
    tree.insert(3, "three");
    tree.insert(7, "seven");
    
    auto* n3 = tree.find(3);
    auto* n5 = tree.find(5);
    auto* n7 = tree.find(7);
    
    // Distance from end (nullptr) to node - should be negative
    EXPECT_EQ(tree.distance(tree.end(), n3), -3);
    EXPECT_EQ(tree.distance(tree.end(), n5), -2);
    EXPECT_EQ(tree.distance(tree.end(), n7), -1);
}

TEST(ThreadedBinaryTree, DistanceAdjacentNodes) {
    ThreadedBinaryTree<int, std::string> tree;
    tree.insert(5, "five");
    tree.insert(3, "three");
    tree.insert(7, "seven");
    
    auto* n3 = tree.find(3);
    auto* n5 = tree.find(5);
    auto* n7 = tree.find(7);
    
    // Inorder: 3, 5, 7
    EXPECT_EQ(tree.distance(n3, n5), 1);
    EXPECT_EQ(tree.distance(n5, n7), 1);
}

TEST(ThreadedBinaryTree, DistanceNonAdjacentNodes) {
    ThreadedBinaryTree<int, std::string> tree;
    tree.insert(5, "five");
    tree.insert(3, "three");
    tree.insert(7, "seven");
    
    auto* n3 = tree.find(3);
    auto* n7 = tree.find(7);
    
    // Inorder: 3, 5, 7
    EXPECT_EQ(tree.distance(n3, n7), 2);
}

TEST(ThreadedBinaryTree, DistanceReverse) {
    ThreadedBinaryTree<int, std::string> tree;
    tree.insert(5, "five");
    tree.insert(3, "three");
    tree.insert(7, "seven");
    
    auto* n3 = tree.find(3);
    auto* n5 = tree.find(5);
    auto* n7 = tree.find(7);
    
    // Reverse direction should return negative distance
    EXPECT_EQ(tree.distance(n5, n3), -1);
    EXPECT_EQ(tree.distance(n7, n5), -1);
    EXPECT_EQ(tree.distance(n7, n3), -2);
}

TEST(ThreadedBinaryTree, DistanceComplexTree) {
    ThreadedBinaryTree<int, std::string> tree;
    tree.insert(10, "ten");
    tree.insert(5, "five");
    tree.insert(15, "fifteen");
    tree.insert(3, "three");
    tree.insert(7, "seven");
    tree.insert(12, "twelve");
    tree.insert(20, "twenty");
    
    auto* n3 = tree.find(3);
    auto* n5 = tree.find(5);
    auto* n7 = tree.find(7);
    auto* n10 = tree.find(10);
    auto* n12 = tree.find(12);
    auto* n15 = tree.find(15);
    auto* n20 = tree.find(20);
    
    // Inorder: 3, 5, 7, 10, 12, 15, 20
    EXPECT_EQ(tree.distance(n3, n20), 6);
    EXPECT_EQ(tree.distance(n5, n15), 4);
    EXPECT_EQ(tree.distance(n7, n12), 2);
    EXPECT_EQ(tree.distance(n10, n10), 0);
    
    // Reverse
    EXPECT_EQ(tree.distance(n20, n3), -6);
    EXPECT_EQ(tree.distance(n15, n5), -4);
}

#include <string>
#include <iterator>
#include <algorithm>
#include <vector>

#include <gtest/gtest.h>

#include "tree.hpp"

using namespace myds;

// ==================== Iterator Tests ====================

TEST(ThreadedBinaryTree, IteratorIncrement) {
    ThreadedBinaryTree<int, std::string> tree;
    tree.insert(5, "five");
    tree.insert(3, "three");
    tree.insert(7, "seven");
    tree.insert(1, "one");
    tree.insert(9, "nine");
    
    auto it = tree.begin();
    ASSERT_NE(it, tree.end());
    EXPECT_EQ(it->first, 1);
    
    ++it;
    EXPECT_EQ(it->first, 3);
    
    ++it;
    EXPECT_EQ(it->first, 5);
    
    ++it;
    EXPECT_EQ(it->first, 7);
    
    ++it;
    EXPECT_EQ(it->first, 9);
    
    ++it;
    EXPECT_EQ(it, tree.end());
}

TEST(ThreadedBinaryTree, IteratorPostIncrement) {
    ThreadedBinaryTree<int, std::string> tree;
    tree.insert(5, "five");
    tree.insert(3, "three");
    tree.insert(7, "seven");
    
    auto it = tree.begin();
    auto old_it = it++;
    
    EXPECT_EQ(old_it->first, 3);
    EXPECT_EQ(it->first, 5);
}

TEST(ThreadedBinaryTree, IteratorDecrement) {
    ThreadedBinaryTree<int, std::string> tree;
    tree.insert(5, "five");
    tree.insert(3, "three");
    tree.insert(7, "seven");
    tree.insert(1, "one");
    tree.insert(9, "nine");
    
    auto it = tree.end();
    
    --it;
    EXPECT_EQ(it->first, 9);
    
    --it;
    EXPECT_EQ(it->first, 7);
    
    --it;
    EXPECT_EQ(it->first, 5);
    
    --it;
    EXPECT_EQ(it->first, 3);
    
    --it;
    EXPECT_EQ(it->first, 1);
    EXPECT_EQ(it, tree.begin());
}

TEST(ThreadedBinaryTree, IteratorPostDecrement) {
    ThreadedBinaryTree<int, std::string> tree;
    tree.insert(5, "five");
    tree.insert(3, "three");
    tree.insert(7, "seven");
    
    auto it = tree.end();
    --it; // На 7
    auto old_it = it--;
    
    EXPECT_EQ(old_it->first, 7);
    EXPECT_EQ(it->first, 5);
}

TEST(ThreadedBinaryTree, IteratorDereference) {
    ThreadedBinaryTree<int, std::string> tree;
    tree.insert(5, "five");
    
    auto it = tree.begin();
    EXPECT_EQ((*it).first, 5);
    EXPECT_EQ((*it).second, "five");
    EXPECT_EQ(it->first, 5);
    EXPECT_EQ(it->second, "five");
}

TEST(ThreadedBinaryTree, IteratorModifyValue) {
    ThreadedBinaryTree<int, std::string> tree;
    tree.insert(5, "five");
    
    auto it = tree.begin();
    it->second = "FIVE";
    
    EXPECT_EQ(tree.find(5)->second, "FIVE");
}

TEST(ThreadedBinaryTree, IteratorEquality) {
    ThreadedBinaryTree<int, std::string> tree;
    tree.insert(5, "five");
    tree.insert(3, "three");
    
    auto it1 = tree.begin();
    auto it2 = tree.begin();
    auto it3 = tree.find(5);
    
    EXPECT_TRUE(it1 == it2);
    EXPECT_FALSE(it1 == it3);
    EXPECT_TRUE(it1 != it3);
    EXPECT_FALSE(it1 != it2);
}

TEST(ThreadedBinaryTree, IteratorRangeBasedFor) {
    ThreadedBinaryTree<int, std::string> tree;
    tree.insert(5, "five");
    tree.insert(3, "three");
    tree.insert(7, "seven");
    tree.insert(1, "one");
    
    std::vector<int> keys;
    for (const auto& pair : tree) {
        keys.push_back(pair.first);
    }
    
    EXPECT_EQ(keys, (std::vector<int>{1, 3, 5, 7}));
}

TEST(ThreadedBinaryTree, IteratorEmptyTree) {
    ThreadedBinaryTree<int, std::string> tree;
    
    EXPECT_EQ(tree.begin(), tree.end());
    
    auto it = tree.begin();
    EXPECT_EQ(it, tree.end());
}

TEST(ThreadedBinaryTree, ConstIterator) {
    ThreadedBinaryTree<int, std::string> tree;
    tree.insert(5, "five");
    tree.insert(3, "three");
    tree.insert(7, "seven");
    
    const auto& ctree = tree;
    
    std::vector<int> keys;
    for (auto it = ctree.begin(); it != ctree.end(); ++it) {
        keys.push_back(it->first);
    }
    
    EXPECT_EQ(keys, (std::vector<int>{3, 5, 7}));
}

TEST(ThreadedBinaryTree, ConstIteratorRangeFor) {
    ThreadedBinaryTree<int, std::string> tree;
    tree.insert(5, "five");
    tree.insert(3, "three");
    
    const auto& ctree = tree;
    
    std::vector<std::string> values;
    for (const auto& pair : ctree) {
        values.push_back(pair.second);
    }
    
    EXPECT_EQ(values, (std::vector<std::string>{"three", "five"}));
}

TEST(ThreadedBinaryTree, ReverseIterator) {
    ThreadedBinaryTree<int, std::string> tree;
    tree.insert(5, "five");
    tree.insert(3, "three");
    tree.insert(7, "seven");
    tree.insert(1, "one");
    tree.insert(9, "nine");
    
    std::vector<int> keys;
    for (auto it = tree.rbegin(); it != tree.rend(); ++it) {
        keys.push_back(it->first);
    }
    
    // Reverse order: 9, 7, 5, 3, 1
    EXPECT_EQ(keys, (std::vector<int>{9, 7, 5, 3, 1}));
}

TEST(ThreadedBinaryTree, ConstReverseIterator) {
    ThreadedBinaryTree<int, std::string> tree;
    tree.insert(5, "five");
    tree.insert(3, "three");
    tree.insert(7, "seven");
    
    const auto& ctree = tree;
    
    std::vector<int> keys;
    for (auto it = ctree.rbegin(); it != ctree.rend(); ++it) {
        keys.push_back(it->first);
    }
    
    EXPECT_EQ(keys, (std::vector<int>{7, 5, 3}));
}

TEST(ThreadedBinaryTree, CBeginCEnd) {
    ThreadedBinaryTree<int, std::string> tree;
    tree.insert(5, "five");
    tree.insert(3, "three");
    
    std::vector<int> keys;
    for (auto it = tree.cbegin(); it != tree.cend(); ++it) {
        keys.push_back(it->first);
    }
    
    EXPECT_EQ(keys, (std::vector<int>{3, 5}));
}

TEST(ThreadedBinaryTree, CRBeginCREnd) {
    ThreadedBinaryTree<int, std::string> tree;
    tree.insert(5, "five");
    tree.insert(3, "three");
    tree.insert(7, "seven");
    
    std::vector<int> keys;
    for (auto it = tree.crbegin(); it != tree.crend(); ++it) {
        keys.push_back(it->first);
    }
    
    EXPECT_EQ(keys, (std::vector<int>{7, 5, 3}));
}

TEST(ThreadedBinaryTree, IteratorStdAlgorithms) {
    ThreadedBinaryTree<int, std::string> tree;
    tree.insert(5, "five");
    tree.insert(3, "three");
    tree.insert(7, "seven");
    tree.insert(1, "one");
    
    // std::find_if
    auto it = std::find_if(tree.begin(), tree.end(), 
        [](const auto& pair) { return pair.first > 4; });
    ASSERT_NE(it, tree.end());
    EXPECT_EQ(it->first, 5);
    
    // std::count_if
    auto count = std::count_if(tree.begin(), tree.end(),
        [](const auto& pair) { return pair.first % 2 == 1; });
    EXPECT_EQ(count, 4); // 1, 3, 5, 7
    
    // std::all_of
    bool all_positive = std::all_of(tree.begin(), tree.end(),
        [](const auto& pair) { return pair.first > 0; });
    EXPECT_TRUE(all_positive);
}

TEST(ThreadedBinaryTree, IteratorAfterInsert) {
    ThreadedBinaryTree<int, std::string> tree;
    tree.insert(5, "five");
    
    auto it = tree.begin();
    EXPECT_EQ(it->first, 5);
    
    tree.insert(3, "three");
    tree.insert(7, "seven");
    
    // Итератор может быть инвалидирован после вставки,
    // но begin() должен указывать на минимальный элемент
    it = tree.begin();
    EXPECT_EQ(it->first, 3);
}

TEST(ThreadedBinaryTree, IteratorSingleElement) {
    ThreadedBinaryTree<int, std::string> tree;
    tree.insert(42, "answer");
    
    auto it = tree.begin();
    EXPECT_EQ(it->first, 42);
    EXPECT_EQ(it->second, "answer");
    
    ++it;
    EXPECT_EQ(it, tree.end());
}

TEST(ThreadedBinaryTree, IteratorBidirectionalTraversal) {
    ThreadedBinaryTree<int, std::string> tree;
    tree.insert(5, "five");
    tree.insert(3, "three");
    tree.insert(7, "seven");
    
    auto it = tree.begin();
    ++it; // На 5
    ++it; // На 7
    
    EXPECT_EQ(it->first, 7);
    
    --it; // Назад на 5
    EXPECT_EQ(it->first, 5);
    
    --it; // Назад на 3
    EXPECT_EQ(it->first, 3);
    EXPECT_EQ(it, tree.begin());
}

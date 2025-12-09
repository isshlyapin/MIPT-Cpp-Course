#include <string>
#include <iterator>

#include <gtest/gtest.h>

#include "tree.hpp"

using namespace myds;

TEST(ThreadedBinaryTree, InsertFindSimple) {
    ThreadedBinaryTree<int, std::string> tree;
    EXPECT_TRUE(tree.insert(5, "five").second);
    EXPECT_TRUE(tree.insert(3, "three").second);
    EXPECT_TRUE(tree.insert(7, "seven").second);
    EXPECT_FALSE(tree.insert(5, "five again").second);
    auto n5 = tree.find(5);
    ASSERT_NE(n5, tree.end());
    EXPECT_EQ(n5->second, "five");
    auto n3 = tree.find(3);
    ASSERT_NE(n3, tree.end());
    EXPECT_EQ(n3->second, "three");
    auto n7 = tree.find(7);
    ASSERT_NE(n7, tree.end());
    EXPECT_EQ(n7->second, "seven");
    EXPECT_EQ(tree.find(42), tree.end());
}

TEST(ThreadedBinaryTree, RemoveSimple) {
    ThreadedBinaryTree<int, std::string> tree;
    tree.insert(5, "five");
    tree.insert(3, "three");
    tree.insert(7, "seven");
    EXPECT_TRUE(tree.remove(3));
    EXPECT_EQ(tree.find(3), tree.end());
    EXPECT_TRUE(tree.remove(5));
    EXPECT_EQ(tree.find(5), tree.end());
    EXPECT_TRUE(tree.remove(7));
    EXPECT_EQ(tree.find(7), tree.end());
    EXPECT_FALSE(tree.remove(42));
}

TEST(ThreadedBinaryTree, InorderTraversal) {
    ThreadedBinaryTree<int, std::string> tree;
    tree.insert(5, "five");
    tree.insert(3, "three");
    tree.insert(7, "seven");
    std::vector<int> keys;
    for (auto it = tree.begin(); it != tree.end(); ++it) {
        keys.push_back(it->first);
    }
    EXPECT_EQ(keys, (std::vector<int>{3, 5, 7}));
}

TEST(ThreadedBinaryTree, CopyAndAssign) {
    ThreadedBinaryTree<int, std::string> tree;
    tree.insert(1, "one");
    tree.insert(2, "two");
    tree.insert(3, "three");
    ThreadedBinaryTree<int, std::string> copy(tree);
    EXPECT_NE(copy.find(1), copy.end());
    EXPECT_NE(copy.find(2), copy.end());
    EXPECT_NE(copy.find(3), copy.end());
    ThreadedBinaryTree<int, std::string> assigned;
    assigned = tree;
    EXPECT_NE(assigned.find(1), assigned.end());
    EXPECT_NE(assigned.find(2), assigned.end());
    EXPECT_NE(assigned.find(3), assigned.end());
}

TEST(ThreadedBinaryTree, LowerBound) {
    ThreadedBinaryTree<int, std::string> tree;
    tree.insert(10, "ten");
    tree.insert(5, "five");
    tree.insert(15, "fifteen");
    tree.insert(3, "three");
    tree.insert(7, "seven");
    
    auto lb3 = tree.lower_bound(3);
    ASSERT_NE(lb3, tree.end());
    EXPECT_EQ(lb3->first, 3);
    
    auto lb4 = tree.lower_bound(4);
    ASSERT_NE(lb4, tree.end());
    EXPECT_EQ(lb4->first, 5);
    
    auto lb6 = tree.lower_bound(6);
    ASSERT_NE(lb6, tree.end());
    EXPECT_EQ(lb6->first, 7);
    
    auto lb20 = tree.lower_bound(20);
    EXPECT_EQ(lb20, tree.end());
}

TEST(ThreadedBinaryTree, UpperBound) {
    ThreadedBinaryTree<int, std::string> tree;
    tree.insert(10, "ten");
    tree.insert(5, "five");
    tree.insert(15, "fifteen");
    tree.insert(3, "three");
    tree.insert(7, "seven");
    
    auto ub3 = tree.upper_bound(3);
    ASSERT_NE(ub3, tree.end());
    EXPECT_EQ(ub3->first, 5);
    
    auto ub5 = tree.upper_bound(5);
    ASSERT_NE(ub5, tree.end());
    EXPECT_EQ(ub5->first, 7);
    
    auto ub14 = tree.upper_bound(14);
    ASSERT_NE(ub14, tree.end());
    EXPECT_EQ(ub14->first, 15);
    
    auto ub20 = tree.upper_bound(20);
    EXPECT_EQ(ub20, tree.end());
}

TEST(ThreadedBinaryTree, ConstLowerBound) {
    ThreadedBinaryTree<int, std::string> tree;
    tree.insert(10, "ten");
    tree.insert(5, "five");
    tree.insert(15, "fifteen");
    
    const auto& ctree = tree;
    auto lb5 = ctree.lower_bound(5);
    ASSERT_NE(lb5, ctree.end());
    EXPECT_EQ(lb5->first, 5);
}

TEST(ThreadedBinaryTree, ConstUpperBound) {
    ThreadedBinaryTree<int, std::string> tree;
    tree.insert(10, "ten");
    tree.insert(5, "five");
    tree.insert(15, "fifteen");
    
    const auto& ctree = tree;
    auto ub5 = ctree.upper_bound(5);
    ASSERT_NE(ub5, ctree.end());
    EXPECT_EQ(ub5->first, 10);
}

TEST(ThreadedBinaryTree, DistanceSameNode) {
    ThreadedBinaryTree<int, std::string> tree;
    tree.insert(5, "five");
    auto n5 = tree.find(5);
    EXPECT_EQ(std::distance(n5, n5), 0);
}

TEST(ThreadedBinaryTree, DistanceToEnd) {
    ThreadedBinaryTree<int, std::string> tree;
    tree.insert(5, "five");
    tree.insert(3, "three");
    tree.insert(7, "seven");
    
    auto n3 = tree.find(3);
    auto n5 = tree.find(5);
    auto n7 = tree.find(7);
    
    // Distance from node to end
    EXPECT_EQ(std::distance(n7, tree.end()), 1);
    EXPECT_EQ(std::distance(n3, tree.end()), 3);
    EXPECT_EQ(std::distance(n5, tree.end()), 2);
}


TEST(ThreadedBinaryTree, DistanceAdjacentNodes) {
    ThreadedBinaryTree<int, std::string> tree;
    tree.insert(5, "five");
    tree.insert(3, "three");
    tree.insert(7, "seven");
    
    auto n3 = tree.find(3);
    auto n5 = tree.find(5);
    auto n7 = tree.find(7);
    
    // Inorder: 3, 5, 7
    EXPECT_EQ(std::distance(n3, n5), 1);
    EXPECT_EQ(std::distance(n5, n7), 1);
}

TEST(ThreadedBinaryTree, DistanceNonAdjacentNodes) {
    ThreadedBinaryTree<int, std::string> tree;
    tree.insert(5, "five");
    tree.insert(3, "three");
    tree.insert(7, "seven");
    
    auto n3 = tree.find(3);
    auto n7 = tree.find(7);
    
    // Inorder: 3, 5, 7
    EXPECT_EQ(std::distance(n3, n7), 2);
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
    
    auto n3 = tree.find(3);
    auto n5 = tree.find(5);
    auto n7 = tree.find(7);
    auto n10 = tree.find(10);
    auto n12 = tree.find(12);
    auto n15 = tree.find(15);
    auto n20 = tree.find(20);
    
    // Inorder: 3, 5, 7, 10, 12, 15, 20
    EXPECT_EQ(std::distance(n3, n20), 6);
    EXPECT_EQ(std::distance(n5, n15), 4);
    EXPECT_EQ(std::distance(n7, n12), 2);
    EXPECT_EQ(std::distance(n10, n10), 0);
}

TEST(ThreadedBinaryTree, RemoveRootWithRightSubtree) {
    ThreadedBinaryTree<int, int> tree;
    tree.insert(10, 10);
    tree.insert(5, 5);
    tree.insert(15, 15);
    tree.insert(12, 12);
    
    // Удаляем корень, у которого есть правое поддерево
    EXPECT_TRUE(tree.remove(10));
    
    // Проверяем, что элементы все еще доступны
    auto it12 = tree.find(12);
    ASSERT_NE(it12, tree.end());
    EXPECT_EQ(it12->first, 12);
    
    auto it5 = tree.find(5);
    ASSERT_NE(it5, tree.end());
    EXPECT_EQ(it5->first, 5);
    
    auto it15 = tree.find(15);
    ASSERT_NE(it15, tree.end());
    EXPECT_EQ(it15->first, 15);
    
    // Проверяем, что удаленный элемент не найден
    EXPECT_EQ(tree.find(10), tree.end());
    
    // Проверяем inorder обход
    std::vector<int> keys;
    for (const auto& pair : tree) {
        keys.push_back(pair.first);
    }
    EXPECT_EQ(keys, (std::vector<int>{5, 12, 15}));
}

TEST(ThreadedBinaryTree, SizeEmpty) {
    ThreadedBinaryTree<int, std::string> tree;
    
    // Пустое дерево
    EXPECT_EQ(tree.size(), 0);
    EXPECT_TRUE(tree.empty());
    
    // Добавляем один элемент
    tree.insert(5, "five");
    EXPECT_EQ(tree.size(), 1);
    EXPECT_FALSE(tree.empty());
    
    // Добавляем еще элементы
    tree.insert(3, "three");
    tree.insert(7, "seven");
    EXPECT_EQ(tree.size(), 3);
    EXPECT_FALSE(tree.empty());
    
    // Попытка добавить дубликат не должна изменить размер
    tree.insert(5, "five again");
    EXPECT_EQ(tree.size(), 3);
    EXPECT_FALSE(tree.empty());
    
    // Удаляем элемент
    tree.remove(3);
    EXPECT_EQ(tree.size(), 2);
    EXPECT_FALSE(tree.empty());
    
    // Удаляем все элементы
    tree.remove(5);
    tree.remove(7);
    EXPECT_EQ(tree.size(), 0);
    EXPECT_TRUE(tree.empty());
    
    // Попытка удалить несуществующий элемент
    tree.remove(42);
    EXPECT_EQ(tree.size(), 0);
    EXPECT_TRUE(tree.empty());
}

TEST(ThreadedBinaryTree, SizeAfterMultipleOperations) {
    ThreadedBinaryTree<int, int> tree;
    
    // Вставляем много элементов
    for (int i = 0; i < 100; ++i) {
        tree.insert(i, i * 10);
    }
    EXPECT_EQ(tree.size(), 100);
    EXPECT_FALSE(tree.empty());
    
    // Удаляем половину
    for (int i = 0; i < 50; ++i) {
        tree.remove(i);
    }
    EXPECT_EQ(tree.size(), 50);
    EXPECT_FALSE(tree.empty());
    
    // Удаляем оставшиеся
    for (int i = 50; i < 100; ++i) {
        tree.remove(i);
    }
    EXPECT_EQ(tree.size(), 0);
    EXPECT_TRUE(tree.empty());
}

TEST(ThreadedBinaryTree, SizeAfterCopyAndMove) {
    ThreadedBinaryTree<int, std::string> tree;
    tree.insert(1, "one");
    tree.insert(2, "two");
    tree.insert(3, "three");
    
    EXPECT_EQ(tree.size(), 3);
    
    // Копирование
    ThreadedBinaryTree<int, std::string> copy(tree);
    EXPECT_EQ(copy.size(), 3);
    EXPECT_EQ(tree.size(), 3); // Оригинал не изменился
    
    // Перемещение
    ThreadedBinaryTree<int, std::string> moved(std::move(copy));
    EXPECT_EQ(moved.size(), 3);
    
    // Присваивание копированием
    ThreadedBinaryTree<int, std::string> assigned;
    assigned = tree;
    EXPECT_EQ(assigned.size(), 3);
    EXPECT_EQ(tree.size(), 3);
    
    // Присваивание перемещением
    ThreadedBinaryTree<int, std::string> move_assigned;
    move_assigned = std::move(assigned);
    EXPECT_EQ(move_assigned.size(), 3);
}

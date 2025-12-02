#include <gtest/gtest.h>

import std;
import linal;

TEST(VectorTest, DefaultConstructedVectorIsEmpty) {
  linal::Vector<int> vec;

  EXPECT_TRUE(vec.empty());
  EXPECT_EQ(vec.size(), 0);
  EXPECT_EQ(vec.capacity(), 0);
  EXPECT_THROW(vec.top(), std::out_of_range);
  EXPECT_THROW(vec.pop(), std::out_of_range);
}

TEST(VectorTest, PushAndTopFollowStackDiscipline) {
  linal::Vector<int> vec;
  vec.push(1);
  vec.push(2);
  vec.push(3);

  EXPECT_EQ(vec.size(), 3);
  EXPECT_EQ(vec.top(), 3);

  vec.pop();
  EXPECT_EQ(vec.top(), 2);
  vec.pop();
  EXPECT_EQ(vec.top(), 1);
}

TEST(VectorTest, GrowIncreasesCapacityExponentially) {
  linal::Vector<int> vec;
  EXPECT_EQ(vec.capacity(), 0);

  vec.push(1);
  EXPECT_EQ(vec.capacity(), 1);

  vec.push(2);
  EXPECT_EQ(vec.capacity(), 3);

  vec.push(3);
  EXPECT_EQ(vec.capacity(), 3);

  vec.push(4);
  EXPECT_EQ(vec.capacity(), 7);
}

TEST(VectorTest, CopyConstructorCreatesDeepCopy) {
  linal::Vector<std::string> original;
  original.push("hello");
  original.push("world");

  const linal::Vector<std::string> copy{original};

  EXPECT_EQ(copy.size(), 2);
  EXPECT_EQ(copy[0], "hello");
  EXPECT_EQ(copy[1], "world");

  original[0] = "bye";
  EXPECT_EQ(copy[0], "hello");
}

TEST(VectorTest, CopyAssignmentReplacesContents) {
  linal::Vector<int> lhs;
  lhs.push(1);

  linal::Vector<int> rhs;
  rhs.push(10);
  rhs.push(20);

  lhs = rhs;

  ASSERT_EQ(lhs.size(), 2);
  EXPECT_EQ(lhs[0], 10);
  EXPECT_EQ(lhs[1], 20);
}

TEST(VectorTest, MoveConstructorTransfersOwnership) {
  linal::Vector<int> original;
  original.push(5);
  original.push(6);

  linal::Vector<int> moved{std::move(original)};

  EXPECT_EQ(moved.size(), 2);
  EXPECT_EQ(moved[0], 5);
  EXPECT_EQ(moved[1], 6);
}

TEST(VectorTest, ReserveDoesNotShrinkBuffer) {
  linal::Vector<int> vec;
  vec.reserve(5);
  EXPECT_EQ(vec.capacity(), 5);

  vec.reserve(3);
  EXPECT_EQ(vec.capacity(), 5);
}

TEST(VectorTest, ResizeAddsAndRemovesElements) {
  linal::Vector<std::string> vec;
  vec.resize(3, "x");

  ASSERT_EQ(vec.size(), 3);
  EXPECT_EQ(vec[0], "x");
  EXPECT_EQ(vec[1], "x");
  EXPECT_EQ(vec[2], "x");

  vec.resize(1);
  EXPECT_EQ(vec.size(), 1);
  EXPECT_EQ(vec[0], "x");
}
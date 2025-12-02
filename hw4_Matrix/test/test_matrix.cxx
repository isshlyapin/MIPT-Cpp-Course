#include <gtest/gtest.h>

import std;
import linal;

TEST(MatrixTest, DefaultConstructorCreatesRectangularMatrix) {
  linal::Matrix<int> mat(3, 4);

  EXPECT_EQ(mat.nrows(), 3);
  EXPECT_EQ(mat.ncols(), 4);
  for (size_t i = 0; i < 3; ++i) {
    for (size_t j = 0; j < 4; ++j) {
      EXPECT_EQ(mat[i][j], 0);
    }
  }
}

TEST(MatrixTest, ConstructorWithValueFillsMatrix) {
  linal::Matrix<double> mat(2, 3, 7.5);

  EXPECT_EQ(mat.nrows(), 2);
  EXPECT_EQ(mat.ncols(), 3);
  for (size_t i = 0; i < 2; ++i) {
    for (size_t j = 0; j < 3; ++j) {
      EXPECT_DOUBLE_EQ(mat[i][j], 7.5);
    }
  }
}

TEST(MatrixTest, IteratorConstructorFillsFromRange) {
  const std::vector<int> data{1, 2, 3, 4, 5, 6};
  linal::Matrix<int> mat(2, 3, data.begin(), data.end());

  EXPECT_EQ(mat.nrows(), 2);
  EXPECT_EQ(mat.ncols(), 3);
  EXPECT_EQ(mat[0][0], 1);
  EXPECT_EQ(mat[0][1], 2);
  EXPECT_EQ(mat[0][2], 3);
  EXPECT_EQ(mat[1][0], 4);
  EXPECT_EQ(mat[1][1], 5);
  EXPECT_EQ(mat[1][2], 6);
}

TEST(MatrixTest, IteratorConstructorPadsWithDefaultWhenRangeIsShort) {
  const std::vector<int> data{10, 20};
  linal::Matrix<int> mat(2, 3, data.begin(), data.end());

  EXPECT_EQ(mat[0][0], 10);
  EXPECT_EQ(mat[0][1], 20);
  EXPECT_EQ(mat[0][2], 0);
  EXPECT_EQ(mat[1][0], 0);
  EXPECT_EQ(mat[1][1], 0);
  EXPECT_EQ(mat[1][2], 0);
}

TEST(MatrixTest, IndexingOperatorAccessesElements) {
  linal::Matrix<int> mat(2, 2, 5);
  
  mat[0][1] = 100;
  mat[1][0] = 200;

  EXPECT_EQ(mat[0][0], 5);
  EXPECT_EQ(mat[0][1], 100);
  EXPECT_EQ(mat[1][0], 200);
  EXPECT_EQ(mat[1][1], 5);
}

TEST(MatrixTest, IsSquareReturnsTrueForSquareMatrix) {
  linal::Matrix<int> square(3, 3);
  EXPECT_TRUE(square.is_square());
}

TEST(MatrixTest, IsSquareReturnsFalseForRectangularMatrix) {
  linal::Matrix<int> rect(2, 4);
  EXPECT_FALSE(rect.is_square());
}

TEST(MatrixTest, DeterminantThrowsOnEmptyMatrix) {
  linal::Matrix<double> empty(0, 0);
  EXPECT_THROW((void)empty.determinant(), std::logic_error);
}

TEST(MatrixTest, DeterminantThrowsOnNonSquareMatrix) {
  linal::Matrix<double> rect(2, 3, 1.0);
  EXPECT_THROW((void)rect.determinant(), std::logic_error);
}

TEST(MatrixTest, DeterminantOf2x2FloatingPointMatrix) {
  const std::vector<double> data{1.0, 2.0, 3.0, 4.0};
  linal::Matrix<double> mat(2, 2, data.begin(), data.end());

  // det = 1*4 - 2*3 = -2
  EXPECT_NEAR(mat.determinant(), -2.0, 1e-9);
}

TEST(MatrixTest, DeterminantOf3x3FloatingPointMatrix) {
  const std::vector<double> data{6.0, 1.0, 1.0,
                                 4.0, -2.0, 5.0,
                                 2.0, 8.0, 7.0};
  linal::Matrix<double> mat(3, 3, data.begin(), data.end());

  // det = -306
  EXPECT_NEAR(mat.determinant(), -306.0, 1e-9);
}

TEST(MatrixTest, DeterminantOfSingularFloatingPointMatrixIsZero) {
  const std::vector<double> data{1.0, 2.0, 3.0,
                                 2.0, 4.0, 6.0,
                                 1.0, 1.0, 1.0};
  linal::Matrix<double> mat(3, 3, data.begin(), data.end());

  EXPECT_NEAR(mat.determinant(), 0.0, 1e-9);
}

TEST(MatrixTest, DeterminantOf2x2IntegralMatrix) {
  const std::vector<int> data{3, 8, 4, 6};
  linal::Matrix<int> mat(2, 2, data.begin(), data.end());

  // det = 3*6 - 8*4 = -14
  EXPECT_EQ(mat.determinant(), -14);
}

TEST(MatrixTest, DeterminantOf3x3IntegralMatrix) {
  const std::vector<int> data{2, -3, 1,
                              2, 0, -1,
                              1, 4, 5};
  linal::Matrix<int> mat(3, 3, data.begin(), data.end());

  // det = 49
  EXPECT_EQ(mat.determinant(), 49);
}

TEST(MatrixTest, DeterminantOfSingularIntegralMatrixIsZero) {
  const std::vector<int> data{1, 2, 3,
                              2, 4, 6,
                              0, 0, 0};
  linal::Matrix<int> mat(3, 3, data.begin(), data.end());

  EXPECT_EQ(mat.determinant(), 0);
}

TEST(MatrixTest, DeterminantOfIdentityMatrixIsOne) {
  const std::vector<double> identity{1.0, 0.0, 0.0,
                                     0.0, 1.0, 0.0,
                                     0.0, 0.0, 1.0};
  linal::Matrix<double> mat(3, 3, identity.begin(), identity.end());

  EXPECT_NEAR(mat.determinant(), 1.0, 1e-9);
}

TEST(MatrixTest, LargeIntegralMatrixDeterminant) {
  const std::vector<int> data{1, 0, 2, -1,
                              3, 0, 0,  5,
                              2, 1, 4, -3,
                              1, 0, 5,  0};
  linal::Matrix<int> mat(4, 4, data.begin(), data.end());

  // det = 30
  EXPECT_EQ(mat.determinant(), 30);
}
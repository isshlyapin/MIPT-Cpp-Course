import std;

import linal;

int main() {
  const std::vector v{1, 2, 3, 4};
  
  const linal::Matrix<double> mat1(2, 2, v.begin(), v.end());

  std::print("det(mat1) = {}\n", mat1.determinant());
}
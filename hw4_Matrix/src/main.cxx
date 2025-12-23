#include <cmath>
#include <cstddef>

#ifdef USE_IMPORT_STD
import std;
#else
#include <print>
#include <vector>
#include <iostream>
#include <stdexcept>
#endif

import linal;

int main() {
  try {
    size_t N = 0;
    if (!(std::cin >> N)) {
      std::println("Failed to read matrix size N");
      return 1;
    }

    std::vector<double> values;
    values.reserve(N * N);
    for (size_t i = 0; i < N * N; ++i) {
      double v = 0;
      if (!(std::cin >> v)) {
        std::println("Failed to read matrix value");
        return 1;
      }
      values.push_back(v);
    }

    const linal::Matrix<double> mat1(N, N, values.begin(), values.end());

    std::println("{}", static_cast<long long>(std::round(mat1.determinant())));
  } catch (const std::exception& ex) {
    std::print("Exception: {}\n", ex.what());
    return 1;
  }
}
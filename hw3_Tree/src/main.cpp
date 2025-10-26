#include <array>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>

#include "tree.hpp"

int main() {
  myds::ThreadedBinaryTree<int, std::string> tree;

  const std::array<std::pair<int, const char*>, 7> entries = {
    std::make_pair(8, "root"),
    std::make_pair(4, "left"),
    std::make_pair(12, "right"),
    std::make_pair(2, "left-left"),
    std::make_pair(6, "left-right"),
    std::make_pair(10, "right-left"),
    std::make_pair(14, "right-right")};

  for (const auto& [key, label] : entries) {
    static_cast<void>(tree.insert(key, std::string(label)));
  }

  std::ofstream dot_file("tree.dot");
  if (!dot_file) {
    std::cerr << "Cannot open tree.dot for writing" << std::endl;
    return 1;
  }

  tree.to_dot(dot_file);
  std::cout << "DOT description stored in tree.dot" << std::endl;
  return 0;
}

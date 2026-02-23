#include <iostream>

#include "tree.hpp"
#include "range_query.hpp"

using namespace myds;
using namespace rq;

int main() {
  ThreadedBinaryTree<int, int> tree;

  char command = '\0';

  try {
    while (std::cin >> command) {
      if (command == 'e') { break; }
      RangeQuery<ThreadedBinaryTree<int, int>>::process_command(tree, command, std::cin, std::cout);
    }
  } catch(const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}

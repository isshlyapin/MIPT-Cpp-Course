#pragma once

#include <iterator>
#include <iostream>
#include <stdexcept>

namespace rq {

template <typename TreeT>
class RangeQuery {
public:
  static void process_command(TreeT& tree, char command, std::istream& in, std::ostream& out) {
    switch (command) {
      case 'k': handle_insert(tree, in); break;
      case 'q': handle_query(tree, in, out); break;
      default:
          throw std::invalid_argument("Unknown command");
    }
  }

private:
  static void handle_insert(TreeT& tree, std::istream& in) {
    int key = 0;
    if (!(in >> key)) { throw std::runtime_error("Failed to read key"); }
    tree.insert(key, key);
  }

  static void handle_query(TreeT& tree, std::istream& in, std::ostream& out) {
    int left_bound = 0;
    int right_bound = 0;
    if (!(in >> left_bound >> right_bound)) {
      throw std::runtime_error("Failed to read range bounds");
    }
    
    size_t count = 0;
    if (right_bound <= left_bound) {
      count = 0;
    } else {
      count = std::distance(tree.lower_bound(left_bound), tree.upper_bound(right_bound));
    }
    out << count << ' ';
  }
};

} // namespace rq

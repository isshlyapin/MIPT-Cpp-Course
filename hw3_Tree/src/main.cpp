#include <iostream>

#include "tree.hpp"

int main() {
  myds::ThreadedBinaryTree<int, int> tree;
  
  char command = '\0';
  
  while (std::cin >> command) {
    if (command == 'k') {
      int key = 0;
      std::cin >> key;
      tree.insert(key, key);
    } else if (command == 'q') {
      int left_bound = 0;
      int right_bound = 0;
      std::cin >> left_bound >> right_bound;
      
      int count = 0;
      // Если R <= L, то ответ 0
      if (right_bound <= left_bound) {
        count = 0;
      } else {
        // Находим первый элемент не меньше L
        auto* start = tree.lower_bound(left_bound);
        
        // Находим первый элемент строго больше R
        auto* end = tree.upper_bound(right_bound);
        
        // Вычисляем расстояние между ними
        count = tree.distance(start, end);
      }
      std::cout << count << ' ';
    }
  }
  
  return 0;
}

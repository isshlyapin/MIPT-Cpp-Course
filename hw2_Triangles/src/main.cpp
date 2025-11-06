#include <ranges>
#include <iostream>

import isshlyapin.triangle;
import isshlyapin.intersection;

int main() {
    try {
        auto triangles = load_triangles();
        auto intersections = find_intersections(triangles);

        // Output results
        for (size_t idx = 0, sz = intersections.size(); idx < sz; ++idx) {
            if (intersections[idx]) {
                std::cout << idx << '\n';
            }
        }
    } catch (const std::invalid_argument& e) {
        std::cerr << "Input error: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Unknown error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
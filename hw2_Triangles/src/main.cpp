#include <array>
#include <iostream>
#include <unordered_set>
#include <vector>
#include <set>

import triangle_intersection;

int main() {
    // Input data
    int N = 0;
    std::cin >> N;

    std::vector<Triangle3> triangles;
    for (int i = 0; i < N; ++i) {
        std::array<Point3, 3> points;
        for (int j = 0; j < 3; ++j) {
            double x{0}, y{0}, z{0};
            std::cin >> x >> y >> z;
            points.at(j) = Point3{.x=x, .y=y, .z=z};
        }
        triangles.emplace_back(points[0], points[1], points[2]);
    }

    // Main cycle
    // std::set<int> intersects;
    std::unordered_set<int> intersects;
    for (int i = 0; i < N; ++i) {
        if (!intersects.contains(i)) {
            for (int j = 0; j < N; ++j) {
                if (i != j && triangles.at(i).intersects(triangles.at(j))) {
                    intersects.insert(i);
                    intersects.insert(j);
                    break;
                }
            }
        }
    }

    // Output results
    // for (auto id: intersects) {
    //     std::cout << id << std::endl;        
    // }

    for (int i = 0; i < 10; ++i) {
        if (!intersects.contains(i)) {
            std::cout << i << std::endl;
        }
    }
}

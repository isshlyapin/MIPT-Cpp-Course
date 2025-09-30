#include <set>
#include <array>
#include <vector>
#include <iostream>

import triangle_intersection;

int main() {
    // Input data
    int N = 0;
    std::cin >> N;

    std::vector<geometry::Triangle3> triangles;
    for (int i = 0; i < N; ++i) {
        std::array<geometry::Point3, 3> points;
        for (int j = 0; j < 3; ++j) {
            double x{0}; double y{0}; double z{0};
            std::cin >> x >> y >> z;
            points.at(j) = geometry::Point3{.x=x, .y=y, .z=z};
        }
        triangles.emplace_back(points[0], points[1], points[2]);
    }

    // Main cycle
    std::set<int> intersects;
    for (int i = 0; i < N; ++i) {
        if (intersects.contains(i)) { continue; }        
        for (int j = 0; j < N; ++j) {
            if (i == j) { continue;} 
            if (triangles.at(i).intersects(triangles.at(j))) {
                intersects.insert(i);
                intersects.insert(j);
                break;
            }
        }
    }

    // Output results
    for (auto id: intersects) {
        std::cout << id << '\n';        
    }
}
module;

#include <array>
#include <vector>
#include <istream>
#include <iostream>

import isshlyapin.point;
import isshlyapin.config;
import isshlyapin.triangle;

export module isshlyapin.intersection;

struct Box3 {
    geometry::Point3 min;
    geometry::Point3 max;

    bool intersects(const geometry::Point3& p) const {
        return (p.x > min.x - EPS) && (p.x < max.x + EPS) &&
               (p.y > min.y - EPS) && (p.y < max.y + EPS) &&
               (p.z > min.z - EPS) && (p.z < max.z + EPS);
    }

    bool intersects(const Box3& other) const {
        return intersects(other.min) || intersects(other.max) || other.intersects(min) || other.intersects(max);
    }
};

export std::vector<geometry::Triangle3> load_triangles(std::istream& in = std::cin) {
    int N = 0;
    if (!(std::cin >> N)) {
        throw std::invalid_argument("Incorrect value for the number of triangles");
    }

    std::vector<geometry::Triangle3> triangles;
    triangles.reserve(N);
    for (int i = 0; i < N; ++i) {
        std::array<geometry::Point3, 3> points;
        for (int j = 0; j < 3; ++j) {
            double x{0}; double y{0}; double z{0};
            if (!(std::cin >> x >> y >> z)) {
                throw std::invalid_argument("Incorrect value for coords");
            }
            points.at(j) = geometry::Point3{x, y, z};
        }
        triangles.emplace_back(points[0], points[1], points[2]);
      }

      return triangles;
}

std::vector<Box3> get_boxes(const std::vector<geometry::Triangle3>& triangles) {
    const size_t tri_numbers = triangles.size();

    std::vector<Box3> boxes;
    boxes.reserve(tri_numbers);
  
    for (size_t i = 0; i < tri_numbers; ++i) {
        const auto& t = triangles[i];
        
        const auto& p0 = t.get_point(0);
        const auto& p1 = t.get_point(1);
        const auto& p2 = t.get_point(2);

        const geometry::Point3 pmin = {
          std::min(std::min(p0.x, p1.x), p2.x),
          std::min(std::min(p0.y, p1.y), p2.y),
          std::min(std::min(p0.z, p1.z), p2.z)
        };
        
        const geometry::Point3 pmax = {
          std::max(std::max(p0.x, p1.x), p2.x),
          std::max(std::max(p0.y, p1.y), p2.y),
          std::max(std::max(p0.z, p1.z), p2.z)
        };

        boxes.push_back(Box3{.min = pmin, .max = pmax});
    }

    return boxes;
}


export std::vector<bool> find_intersections(const std::vector<geometry::Triangle3>& triangles) {  
    const size_t tri_numbers = triangles.size();

    const auto boxes = get_boxes(triangles);
  
    std::vector<bool> results(tri_numbers, false);

    // Main cycle
    for (size_t i = 0; i < tri_numbers; ++i) {
        if (results[i]) { continue; }
        for (size_t j = i + 1; j < tri_numbers; ++j) {
            if (!boxes[i].intersects(boxes[j])) { continue; }
            if (triangles[i].intersects(triangles[j])) {
                results[i] = true;
                results[j] = true;
            }
        }
    }  

    return results;
}
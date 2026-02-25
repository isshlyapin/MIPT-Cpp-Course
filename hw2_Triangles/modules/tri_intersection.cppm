module;

#include <cmath>
#include <array>
#include <vector>
#include <istream>
#include <iostream>
#include <algorithm>

export module isshlyapin.intersection;

import isshlyapin.point;
import isshlyapin.config;
import isshlyapin.triangle;

using Point3    = geometry::Point3<double>;
using Triangle3 = geometry::Triangle3<double>;

struct Box3 {
    Point3 min;
    Point3 max;

    bool intersects(const Box3& other) const {
        return ((min.x - EPS<double>) < other.max.x && (max.x + EPS<double>) > other.min.x) &&
               ((min.y - EPS<double>) < other.max.y && (max.y + EPS<double>) > other.min.y) &&
               ((min.z - EPS<double>) < other.max.z && (max.z + EPS<double>) > other.min.z);
    }

    void extend(const Box3& other) {
        min.x = std::min(min.x, other.min.x);
        min.y = std::min(min.y, other.min.y);
        min.z = std::min(min.z, other.min.z);

        max.x = std::max(max.x, other.max.x);
        max.y = std::max(max.y, other.max.y);
        max.z = std::max(max.z, other.max.z);
    }
};

struct GridParams {
    double dx, dy, dz;
    size_t n_cells_x, n_cells_y, n_cells_z, total_cells;
};

export std::vector<Triangle3> load_triangles(std::istream& in = std::cin) {
    int N = 0;
    if (!(in >> N)) {
        throw std::invalid_argument("Incorrect value for the number of triangles");
    }
    if (N < 0) {
        throw std::invalid_argument("The number of triangles must be greater than 0");
    }

    std::vector<Triangle3> triangles;
    triangles.reserve(N);
    for (int i = 0; i < N; ++i) {
        std::array<Point3, 3> points;
        for (int j = 0; j < 3; ++j) {
            double x{0}; double y{0}; double z{0};
            if (!(in >> x >> y >> z)) {
                throw std::invalid_argument("Incorrect coordinate values");
            }
            points.at(j) = Point3{x, y, z};
        }
        triangles.emplace_back(points[0], points[1], points[2]);
    }

    return triangles;
}

std::vector<Box3> get_boxes(const std::vector<Triangle3>& triangles) {
    const size_t tri_numbers = triangles.size();

    std::vector<Box3> boxes;
    boxes.reserve(tri_numbers);
  
    for (size_t i = 0; i < tri_numbers; ++i) {
        const auto& t = triangles[i];
        
        const auto& p0 = t.get_point(0);
        const auto& p1 = t.get_point(1);
        const auto& p2 = t.get_point(2);

        const Point3 pmin = {
          std::min({p0.x, p1.x, p2.x}),
          std::min({p0.y, p1.y, p2.y}),
          std::min({p0.z, p1.z, p2.z})
        };
        
        const Point3 pmax = {
          std::max({p0.x, p1.x, p2.x}),
          std::max({p0.y, p1.y, p2.y}),
          std::max({p0.z, p1.z, p2.z})
        };

        boxes.push_back(Box3{.min = pmin, .max = pmax});
        if (i == 0) {
            boxes.push_back(Box3{.min = pmin, .max = pmax});
        } else {
            boxes[0].extend(boxes.back());
        }
    }

    return boxes;
}

GridParams calculate_grid_params(const std::vector<Box3>& boxes, size_t tri_numbers, size_t tri_density) {
    const size_t base_n_cells = static_cast<size_t>(std::cbrt((tri_numbers / tri_density) + 1));

    double max_tri_depth  = 0.0;
    double max_tri_width  = 0.0;
    double max_tri_height = 0.0;

    for (size_t i = 1, sz = boxes.size(); i < sz; ++i) {
        const auto& box = boxes[i];
        max_tri_depth  = std::max(max_tri_depth,  box.max.z - box.min.z);
        max_tri_width  = std::max(max_tri_width,  box.max.x - box.min.x);
        max_tri_height = std::max(max_tri_height, box.max.y - box.min.y);
    }

    const double base_cell_depth  = (boxes[0].max.z - boxes[0].min.z) / base_n_cells;
    const double base_cell_width  = (boxes[0].max.x - boxes[0].min.x) / base_n_cells;
    const double base_cell_height = (boxes[0].max.y - boxes[0].min.y) / base_n_cells;

    const double min_cell_width  = max_tri_width  * 1.05;
    const double min_cell_depth  = max_tri_depth  * 1.05;
    const double min_cell_height = max_tri_height * 1.05;

    const double dx = std::max(base_cell_width,  min_cell_width);
    const double dz = std::max(base_cell_depth,  min_cell_depth);
    const double dy = std::max(base_cell_height, min_cell_height);

    const size_t n_cells_x = (dx > EPS<double>) ? static_cast<size_t>(std::ceil((boxes[0].max.x - boxes[0].min.x) / dx)) : 1;
    const size_t n_cells_y = (dy > EPS<double>) ? static_cast<size_t>(std::ceil((boxes[0].max.y - boxes[0].min.y) / dy)) : 1;
    const size_t n_cells_z = (dz > EPS<double>) ? static_cast<size_t>(std::ceil((boxes[0].max.z - boxes[0].min.z) / dz)) : 1;

    const size_t total_cells = n_cells_x * n_cells_y * n_cells_z;

    return {.dx=dx, .dy=dy, .dz=dz, .n_cells_x=n_cells_x, .n_cells_y=n_cells_y, .n_cells_z=n_cells_z, .total_cells=total_cells};
}

std::vector<std::vector<size_t>> map_triangles_to_cells(const std::vector<Box3>& boxes, const GridParams& params) {
    std::vector<std::vector<size_t>> cell_triangles(params.total_cells);

    for (size_t i = 1, sz = boxes.size(); i < sz; ++i) {
        const auto& box = boxes[i];

        size_t x_min_cell = (params.dx > EPS<double>) ? static_cast<size_t>((box.min.x - boxes[0].min.x) / params.dx) : 0;
        size_t y_min_cell = (params.dy > EPS<double>) ? static_cast<size_t>((box.min.y - boxes[0].min.y) / params.dy) : 0;
        size_t z_min_cell = (params.dz > EPS<double>) ? static_cast<size_t>((box.min.z - boxes[0].min.z) / params.dz) : 0;

        size_t x_max_cell = (params.dx > EPS<double>) ? static_cast<size_t>((box.max.x - boxes[0].min.x) / params.dx) : 0;
        size_t y_max_cell = (params.dy > EPS<double>) ? static_cast<size_t>((box.max.y - boxes[0].min.y) / params.dy) : 0;
        size_t z_max_cell = (params.dz > EPS<double>) ? static_cast<size_t>((box.max.z - boxes[0].min.z) / params.dz) : 0;

        x_min_cell = std::min(x_min_cell, params.n_cells_x - 1);
        y_min_cell = std::min(y_min_cell, params.n_cells_y - 1);
        z_min_cell = std::min(z_min_cell, params.n_cells_z - 1);

        x_max_cell = std::min(x_max_cell, params.n_cells_x - 1);
        y_max_cell = std::min(y_max_cell, params.n_cells_y - 1);
        z_max_cell = std::min(z_max_cell, params.n_cells_z - 1);

        for (size_t x = x_min_cell; x <= x_max_cell; ++x) {
            for (size_t y = y_min_cell; y <= y_max_cell; ++y) {
                for (size_t z = z_min_cell; z <= z_max_cell; ++z) {
                    const size_t cell_index = (x * params.n_cells_y * params.n_cells_z) + (y * params.n_cells_z) + z;
                    cell_triangles[cell_index].push_back(i-1);
                }
            }
        }
    }

    return cell_triangles;
}

export std::vector<bool> find_intersections(const std::vector<Triangle3>& triangles) {
    const size_t tri_numbers = triangles.size();
    
    const auto boxes = get_boxes(triangles);
    
    const GridParams params = calculate_grid_params(boxes, tri_numbers, 10);
    
    const auto cell_triangles = map_triangles_to_cells(boxes, params);
    
    std::vector<bool> results(tri_numbers, false);
    for (const auto& tri_idxs : cell_triangles) {
        for (size_t i = 0, sz = tri_idxs.size(); i < sz; ++i) {
            for (size_t j = i + 1; j < sz; ++j) {
                if (triangles[tri_idxs[i]].intersects(triangles[tri_idxs[j]])) {
                    results[tri_idxs[i]] = true;
                    results[tri_idxs[j]] = true;
                }
            }
        }
    }

    return results;
}
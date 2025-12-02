module;

#include <array>
#include <cmath>
#include <ranges>
#include <vector>
#include <utility>
#include <cassert>
#include <cstdint>
#include <algorithm>
#include <stdexcept>

export module isshlyapin.triangle;

import isshlyapin.line;
import isshlyapin.point;
import isshlyapin.plane;
import isshlyapin.vector;
import isshlyapin.config;
import isshlyapin.line_segment;

namespace geometry {

template<std::floating_point T>
Point2<T> project_point3_to_point2(const Point3<T>& p, const Vector3<T>& normal);

export
template<std::floating_point T>
class Triangle2 {
public:
    Triangle2(const Point2<T>& p1, const Point2<T>& p2, const Point2<T>& p3)
      : points_{p1, p2, p3} {}

    enum class Type : std::uint8_t {
        Triangle = 0,
        LineSegment = 1,
        Point = 2
    };

    bool is_degenerate() const {
        const Vector2<T> v1{points_[0], points_[1]};
        const Vector2<T> v2{points_[0], points_[2]};

        return std::fabs(v1.cross(v2)) < EPS<T>;
    }

    Type get_type() const {
        if (is_degenerate()) { 
            const Vector2<T> v1{points_[0], points_[1]};
            const Vector2<T> v2{points_[0], points_[2]};

            if (v1.length() < EPS<T> && v2.length() < EPS<T>) {
                return Type::Point;
            }

            return Type::LineSegment;
        }
        return Type::Triangle; 
    }

    bool contains_point(const Point2<T>& p) const {
        switch (get_type()) {
            case Type::Triangle:
                return contains_point_triangle(p);
            case Type::LineSegment:
                return contains_point_segment(p);
            case Type::Point:
                return contains_point_point(p);
            default:
                throw std::invalid_argument("Unknown type Triangle2");
        }
    }

    bool intersects(const Triangle2& other_t) const {
        for (int i = 0; i < 3; ++i) {
            const LineSegment2<T> ls1{points_.at(i), points_.at((i + 1) % 3)};
            for (int j = 0; j < 3; ++j) {
                const LineSegment2<T> ls2{other_t.get_point(j), other_t.get_point((j + 1) % 3)};
                if (ls1.intersects(ls2)) {
                    return true;
                }
            }
        }

        if (contains_point(other_t.get_point(0)))  { return true; }
        if (other_t.contains_point(points_.at(0))) { return true; }

        return false;
    }

    bool intersects(const LineSegment2<T>& ls) const {
        for (int i = 0; i < 3; ++i) {
            const LineSegment2<T> ls1{points_.at(i), points_.at((i + 1) % 3)};
            if (ls1.intersects(ls)) { return true; }
        }

        return contains_point(ls.get_point(0));
    }

    const Point2<T>& get_point(int index) const {
        return points_.at(index);
    }

    const auto& get_points() const {
        return points_;
    }

private:
    bool contains_point_point(const Point2<T>& p) const {
        return Vector2<T>{points_[0], p}.length() < EPS<T>;
    }    

    bool contains_point_segment(const Point2<T>& p) const {
        return LineSegment2<T>{points_[0], points_[1]}.contains_point(p) ||
               LineSegment2<T>{points_[0], points_[2]}.contains_point(p);
    }

    bool contains_point_triangle(const Point2<T>& p) const {
        for (const auto& v: points_) {
            if (Vector2<T>{v, p}.length() < EPS<T>) { return true; }
        }
    
        for (int i = 0; i < 3; ++i) {
            if (LineSegment2<T>{points_.at(i), points_.at((i+1)%3)}.contains_point(p)) {
                return true;
            }
        }

        int count = 0;
        for (int i = 0; i < 3; ++i) {
            const Point2<T>& a = points_.at(i);
            const Point2<T>& b = points_.at((i + 1) % 3);

            if (p.y < std::max(a.y, b.y) && p.y > std::min(a.y, b.y)) {
                const T x_int = a.x + ((b.x - a.x) * ((a.y - p.y) / (a.y - b.y)));

                if (p.x < x_int) {
                    ++count;
                }
            }
        }
        return ((count % 2) == 1);
    }

    std::array<Point2<T>, 3> points_;
};

export
template<std::floating_point T>
class Triangle3 {
public:
    Triangle3(const Point3<T>& p1, const Point3<T>& p2, const Point3<T>& p3)
      : plane_(p1, p2, p3), points_{p1, p2, p3} {}

    enum class Type : std::uint8_t {
        Triangle    = 0,
        LineSegment = 1,
        Point       = 2
    };

    bool is_valid() const {
        Vector3<T> v1{points_[0], points_[1]};
        Vector3<T> v2{points_[0], points_[2]};
        return v1.cross(v2).length() > EPS<T>;
    }

    bool is_degenerate() const {
        return !is_valid();
    }

    Type get_type() const {
        if (is_degenerate()) {
            const Vector3<T> v1{points_[0], points_[1]};
            const Vector3<T> v2{points_[0], points_[2]};

            if (v1.length() < EPS<T> && v2.length() < EPS<T>) { return Type::Point; }

            return Type::LineSegment;
        }

        return Type::Triangle;
    }

    bool intersects(const Triangle3& other_t) const {
        if (is_degenerate() || other_t.is_degenerate()) {
            return degenerate_intersects(other_t);
        }

        if (is_coplanar(other_t)) {
            return coplanar_intersects(other_t);
        }

        if (intersects(other_t.get_plane()) && other_t.intersects(plane_)) {
            return intersects_on_plane_intersection_line(other_t);
        }
        
        return false;
    }

    bool intersects(const Plane3<T>& plane) const {
        if (!plane.is_valid()) {
             throw std::invalid_argument("Degenerate plane");
        }

        const auto distances = distances_to_plane(*this, plane);

        const auto [min, max] = std::ranges::minmax_element(distances);

        const bool is_intersect = *min < -EPS<T> && *max > EPS<T>; 

        const bool is_lie = std::ranges::any_of(
            distances, 
            [](T d){ return std::fabs(d) < EPS<T>; }
        );

        return  is_intersect || is_lie;
    }

    bool intersects(const LineSegment3<T>& ls) const {
        const T d1 = plane_.distance_to_point(ls.get_point(0));
        const T d2 = plane_.distance_to_point(ls.get_point(1));

        if (d1 * d2 < -EPS<T>) {
            const Vector3<T> intersection_vec = 
                Vector3<T>{ls.get_point(0)} + 
                Vector3<T>{
                    ls.get_point(0), 
                    ls.get_point(1)
                }.scale(std::fabs(d1)/(std::fabs(d1) + std::fabs(d2)));
            
            return intersects(Point3<T>{intersection_vec.x, intersection_vec.y, intersection_vec.z});
        }
        if (std::fabs(d1) < EPS<T> && std::fabs(d2) < EPS<T>) {
            const Triangle2<T>    tri2 = project_to_2d(plane_.get_normal());
            const LineSegment2<T> ls2  = ls.project_to_2d(plane_.get_normal());

            return tri2.intersects(ls2);
        }
        return false;
    }

    bool intersects(const Point3<T>& p) const {
        if (plane_.distance_to_point(p) > EPS<T>) {
            return false;
        }
        const Vector3<T> normal = plane_.get_normal();
        const Triangle2<T> tri2 = project_to_2d(normal);

        const Point2<T> p2 = project_point3_to_point2<T>(p, normal);

        return tri2.contains_point(p2);
    }

    bool is_coplanar(const Triangle3& other_t) const {
        const auto distances = distances_to_plane(other_t, plane_);

        return std::ranges::all_of(distances, [](T d){ return std::fabs(d) < EPS<T>; });
    }

    const Point3<T>& get_point(int index) const {
        return points_.at(index);
    }

    const auto& get_points() const {
        return points_;
    }

    const Plane3<T>& get_plane() const {
        return plane_;
    }
    
    Triangle2<T> project_to_2d(const Vector3<T>& normal) const {
        // Выбираем ось с максимальным влиянием в нормали
        const T ax = std::fabs(normal.x);
        const T ay = std::fabs(normal.y);
        const T az = std::fabs(normal.z);

        std::array<Point2<T>, 3> result{};

        if (ax > ay && ax > az) {
            // отбросить X → остаются (y, z)
            for (int i = 0; i < 3; ++i) {
                result.at(i) = Point2<T>(points_.at(i).y, points_.at(i).z);
            }
        } else if (ay > ax && ay > az) {
            // отбросить Y → остаются (x, z)
            for (int i = 0; i < 3; ++i) {
                result.at(i) = Point2<T>(points_.at(i).x, points_.at(i).z);
            }
        } else {
            // отбросить Z → остаются (x, y)
            for (int i = 0; i < 3; ++i) {
                result.at(i) = Point2<T>(points_.at(i).x, points_.at(i).y);
            }
        }

        return Triangle2<T>{result[0], result[1], result[2]};
    }

private:
    bool degenerate_intersects(const Triangle3& other_t) const {
        if (get_type() == Type::Triangle) {
            if (other_t.get_type() == Type::Point) {
                return intersects(other_t.get_point(0));
            }
            return intersects(other_t.degenerate_segment());
        }
        if (get_type() == Type::Point) {
            if (other_t.get_type() == Type::Triangle) {
                return other_t.intersects(points_[0]);
            }
            if (other_t.get_type() == Type::Point) {
                return points_[0] == other_t.get_point(0);
            }

            return other_t.degenerate_segment().contains_point(points_[0]);
        }

        if (get_type() == Type::LineSegment) {
            if (other_t.get_type() == Type::Triangle) {
                return other_t.intersects(degenerate_segment());
            }
            if (other_t.get_type() == Type::Point) {
                return degenerate_segment().contains_point(other_t.get_point(0));
            }

            return degenerate_segment().intersects(other_t.degenerate_segment());
        }

        throw std::logic_error("Unknown type Triangle3");
    }

    LineSegment3<T> degenerate_segment() const {
        LineSegment3<T> max_ls{Point3<T>{}, Point3<T>{}};
        T max_length = 0;
        for (int i = 0; i < 3; ++i) {
            const LineSegment3<T> ls{points_.at(i), points_.at((i+1)%3)};
            const T ls_length = ls.length();
            if (ls_length > max_length) {
                max_ls = ls;
                max_length = ls_length;
            }
        }
        return max_ls;
    }

    bool intersects_on_plane_intersection_line(const Triangle3& other_t) const {
        const Vector3<T> intersection_vec = 
            plane_.get_normal().cross(other_t.get_plane().get_normal());

        auto intersection_proj_other_t = 
            compute_projection_on_plane_intesection_vector(
                other_t, plane_, intersection_vec
            );

        auto intersection_proj = 
            compute_projection_on_plane_intesection_vector(
                *this, other_t.get_plane(), intersection_vec
            );

        const bool overlap = 
            std::max(intersection_proj[0], intersection_proj_other_t[0]) <= 
            std::min(intersection_proj[1], intersection_proj_other_t[1]);
        
        return overlap;
    }

    bool coplanar_intersects(const Triangle3& other_t) const {
        const Triangle2<T> t1 = project_to_2d(plane_.get_normal());
        const Triangle2<T> t2 = other_t.project_to_2d(plane_.get_normal());

        return t1.intersects(t2);
    }
    
    static std::array<T, 2> compute_projection_on_plane_intesection_vector(
        const Triangle3& tri, const Plane3<T>& plane, const Vector3<T>& dir) 
    {
        auto distances = distances_to_plane(tri, plane);

        auto zeroes_indices = std::ranges::views::iota(0, 3)
            | std::ranges::views::filter([&](int i){ return std::fabs(distances.at(i)) < EPS<T>; });

        const std::vector<int> zi{zeroes_indices.begin(), zeroes_indices.end()};

        std::array<T, 2> projections{};

        if (zi.size() == 2) {
            projections[0] = dir.dot(tri.get_point(zi[0]));
            projections[1] = dir.dot(tri.get_point(zi[1]));
        }

        if (zi.size() == 1) {
            const int i = zi[0];
            if (distances.at((i+1)%3) * distances.at((i+2)%3) < -EPS<T>) {
                projections[0] = dir.dot(tri.get_point(i));
                projections[1] = dir.dot(
                    intersect_edge_with_plane(
                        tri.get_point((i+1)%3), distances.at((i+1)%3), 
                        tri.get_point((i+2)%3), distances.at((i+2)%3)
                    )
                );
            } else {
                projections[0] = dir.dot(tri.get_point(i));
                projections[1] = dir.dot(tri.get_point(i));
            }
        }

        if (zi.empty()) {
            for (int i = 0; i < 3; ++i) {
                const int j = (i + 1) % 3;
                const int k = (i + 2) % 3;
                if (distances.at(i) * distances.at(j) < -EPS<T> && 
                    distances.at(i) * distances.at(k) < -EPS<T>) {
                    projections[0] = dir.dot(
                        intersect_edge_with_plane(
                            tri.get_point(i), distances.at(i),
                            tri.get_point(j), distances.at(j)
                        )
                    );
                    projections[1] = dir.dot(
                        intersect_edge_with_plane(
                            tri.get_point(i), distances.at(i),
                            tri.get_point(k), distances.at(k)
                        )
                    );  
                }
            }
        }

        if (projections[0] > projections[1]) {
            std::swap(projections[0], projections[1]);
        }

        return projections;
    }

    static std::array<double, 3> distances_to_plane(const Triangle3& tri, const Plane3<T>& plane) {
        return {
            plane.distance_to_point(tri.get_point(0)),
            plane.distance_to_point(tri.get_point(1)),
            plane.distance_to_point(tri.get_point(2))
        };
    }

    static Vector3<T> intersect_edge_with_plane(const Point3<T>& p1, T d1,
                                             const Point3<T>& p2, T d2) 
    {
        Vector3<T> edge{p1, p2};
        const T t = d1 / (d1 - d2);  // параметр пересечения
        return Vector3<T>{p1} + edge.scale(t);
    }

    Plane3<T> plane_;
    std::array<Point3<T>, 3> points_;
};

template<std::floating_point T>
Point2<T> project_point3_to_point2(const Point3<T>& p, const Vector3<T>& normal) {
    // Выбираем ось с максимальным влиянием в нормали
    const T ax = std::fabs(normal.x);
    const T ay = std::fabs(normal.y);
    const T az = std::fabs(normal.z);

    if (ax > ay && ax > az) {
        // отбросить X → остаются (y, z)
        return Point2<T>{p.y, p.z};
    } else if (ay > ax && ay > az) {
        // отбросить Y → остаются (x, z)
        return Point2<T>{p.x, p.z};
    } else {
        // отбросить Z → остаются (x, y)
        return Point2<T>{p.x, p.y};
    }
}

} // namespace geometry
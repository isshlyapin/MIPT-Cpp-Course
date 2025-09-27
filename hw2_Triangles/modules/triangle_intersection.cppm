module;

#include <array>
#include <cmath>
#include <utility>
#include <cassert>
#include <algorithm>
#include <stdexcept>

export module triangle_intersection;

const double EPS = 1e-6;

export struct Point2;
export struct Vector2;
export class  LineSegment2;
export class  Triangle2;

export struct Point3;
export class  Vector3;
export class  Plane3;
export class  Triangle3;

struct Point2 {
    Point2(double x = 0, double y = 0) : x{x}, y{y} {}

    double x, y;
};

struct Vector2 {
    Vector2(double x = 0, double y = 0) : x{x}, y{y} {}
    
    Vector2(const Point2& p1, const Point2& p2) : Vector2(p2.x - p1.x, p2.y - p1.y) {}
    
    Vector2(const Point2& p) : Vector2(p.x, p.y) {}

    double dot(const Vector2& other_v) const {
        return (x * other_v.x) + (y * other_v.y);
    }

    double cross(const Vector2& other_v) const {
        return (x * other_v.y) - (y * other_v.x);
    }

    double length() const {
        return std::sqrt((x * x) + (y * y));
    }

    double length_square() const {
        return (x * x) + (y * y);
    }

    double x, y;
};

class LineSegment2 {
public:
    LineSegment2(const Point2& p1, const Point2& p2) : points_{p1, p2} {
        if (!is_valid()) {
            throw std::invalid_argument("Degenerate LineSegment: points equal");
        }
    }

    bool is_valid() const {
        return std::fabs(points_[0].x - points_[1].x) >= EPS ||
               std::fabs(points_[0].y - points_[1].y) >= EPS;
    }

    bool intersects(const LineSegment2& other_ls) const {
        const Point2& a = points_[0];
        const Point2& b = points_[1];
        const Point2& c = other_ls.get_point(0);
        const Point2& d = other_ls.get_point(1);

        auto cross = [](const Point2& p1, const Point2& p2, const Point2& p3) {
            return ((p2.x - p1.x) * (p3.y - p1.y)) - ((p2.y - p1.y) * (p3.x - p1.x));
        };

        const double d1 = cross(a, b, c);
        const double d2 = cross(a, b, d);
        const double d3 = cross(c, d, a);
        const double d4 = cross(c, d, b);

        if (((d1 > EPS && d2 < -EPS) || (d1 < -EPS && d2 > EPS)) &&
            ((d3 > EPS && d4 < -EPS) || (d3 < -EPS && d4 > EPS))) {
            return true;
        }

        auto on_segment = [](const Point2& p, const Point2& q, const Point2& r) {
            return std::min(p.x, q.x) - EPS <= r.x && r.x <= std::max(p.x, q.x) + EPS &&
                   std::min(p.y, q.y) - EPS <= r.y && r.y <= std::max(p.y, q.y) + EPS;
        };
        
        if (std::fabs(d1) < EPS && on_segment(a, b, c)) return true;
        if (std::fabs(d2) < EPS && on_segment(a, b, d)) return true;
        if (std::fabs(d3) < EPS && on_segment(c, d, a)) return true;
        if (std::fabs(d4) < EPS && on_segment(c, d, b)) return true;
        
        return false;
    }

    const Point2& get_point(int index) const {
        return points_.at(index);
    }

    const auto& get_points() const {
        return points_;
    }
private:
    std::array<Point2, 2> points_;
};

class Triangle2 {
public:
    Triangle2(const Point2& p1, const Point2& p2, const Point2& p3)
      : points_{p1, p2, p3} {
        if (!is_valid()) {
            throw std::invalid_argument("Degenerate triangle: points are collinear or equal");
        }
      }

    bool is_valid() const {
        const Vector2 v1{points_[0], points_[1]};
        const Vector2 v2{points_[0], points_[2]};

        return std::fabs(v1.cross(v2)) > EPS;
    }

    bool is_point_in_triangle(const Point2& p) const {
        int count = 0;
        for (int i = 0; i < 3; ++i) {
            const Point2& a = points_[i];
            const Point2& b = points_[(i + 1) % 3];

            if (p.y < std::max(a.y, b.y) && p.y > std::min(a.y, b.y)) {
                const double x_int = a.x + ((b.x - a.x) * ((a.y - p.y) / (a.y - b.y)));

                if (p.x < x_int) {
                    ++count;
                }
            }
        }

        return ((count % 2) == 1);
    }

    bool intersects(const Triangle2& other_t) const {
        for (int i = 0; i < 3; ++i) {
            LineSegment2 ls1{points_.at(i), points_.at((i + 1) % 3)};
            for (int j = 0; j < 3; ++j) {
                LineSegment2 ls2{other_t.get_point(j), other_t.get_point((j + 1) % 3)};
                if (ls1.intersects(ls2)) {
                    return true;
                }
            }
        }

        if (is_point_in_triangle(other_t.get_point(0)))  return true;
        if (other_t.is_point_in_triangle(points_.at(0))) return true;

        return false;
    }

    const Point2& get_point(int index) const {
        return points_.at(index);
    }

    const auto& get_points() const {
        return points_;
    }

private:
    std::array<Point2, 3> points_;
};

struct Point3 {
    double x{0}, y{0}, z{0};
};

struct Vector3 {
    Vector3(double x = 0, double y = 0, double z = 0)
      : x(x), y(y), z(z) {}

    Vector3(const Point3& p) : Vector3(p.x, p.y, p.z) {}

    Vector3(const Point3& p1, const Point3& p2) 
      : Vector3(p2.x - p1.x, p2.y - p1.y, p2.z - p1.z) {}
        
    Vector3 cross(const Vector3& other_v) const {
        Vector3 res_v{
            (y * other_v.z) - (z * other_v.y),
            (z * other_v.x) - (x * other_v.z),
            (x * other_v.y) - (y * other_v.x)
        };

        return res_v; 
    }

    const Vector3& scale(double scl) {
        x *= scl;
        y *= scl;
        z *= scl;

        return *this;
    }

    double dot(const Vector3& other_v) const {
        return (x * other_v.x) + (y * other_v.y) + (z * other_v.z);
    }

    double length() const {
        return std::sqrt((x * x) + (y * y) + (z * z));
    }

    double length_square() const {
        return (x * x) + (y * y) + (z * z);
    }

    Vector3 operator+(const Vector3& other) const {
        return {x + other.x, y + other.y, z + other.z};
    }

    Vector3 operator-(const Vector3& other) const {
        return {x - other.x, y - other.y, z - other.z};
    }

    Vector3& operator+=(const Vector3& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    Vector3& operator-=(const Vector3& other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }
    
    double x, y, z;
};

class Plane3 {
public:
    Plane3(Point3 p1, Point3 p2, Point3 p3) {
        Vector3 vec1{p1, p2};
        Vector3 vec2{p1, p3};

        normal = vec1.cross(vec2);
        d = - normal.dot(Vector3{p1});

        if (!is_valid()) {
            throw std::invalid_argument("Degenerate plane: points equal");
        }
    }

    Plane3(const Vector3& v, double d = 0) : normal{v}, d{d} {}

    bool is_valid() const {
        return normal.length() > EPS;
    }

    double distance_to_point(const Point3& p) const {
        return normal.dot(p) + d;
    }

    const Vector3& get_normal() const {
        return normal;
    }

private:
    double d;
    Vector3 normal;
};

class Triangle3 {
public:
    Triangle3(const Point3& p1, const Point3& p2, const Point3& p3)
      : plane_(p1, p2, p3), points_{p1, p2, p3} {
        if (!is_valid()) {
            throw std::invalid_argument("Degenerate triangle: points are collinear");
        }
      }

    bool is_valid() const {
        Vector3 v1{points_[0], points_[1]};
        Vector3 v2{points_[0], points_[2]};
        return v1.cross(v2).length() > EPS;
    }

    bool intersects(const Triangle3& other_t) const {
        // Triangles from the same plane
        if (is_coplanar(other_t)) {
            return intersects_2d(other_t);
        }

        if (intersects(other_t.get_plane()) && other_t.intersects(plane_)) {
            return intersects_on_plane_intersection_line(other_t);
        }
        
        return false;
    }

    bool intersects(const Plane3& plane) const {
        const auto distances = distances_to_plane(*this, plane);

        const auto [min, max] = std::ranges::minmax_element(distances);

        const bool is_intersect = *min < -EPS && *max > EPS; 

        const bool is_lie = std::ranges::any_of(
            distances, 
            [](double d){ return std::fabs(d) < EPS; }
        );

        return  is_intersect || is_lie;
    }

    bool is_coplanar(const Triangle3& other_t) const {
        const auto distances = distances_to_plane(other_t, plane_);

        return std::ranges::all_of(distances, [](double d){ return std::fabs(d) < EPS; });
    }

    const Point3& get_point(int index) const {
        return points_.at(index);
    }

    const auto& get_points() const {
        return points_;
    }

    const Plane3& get_plane() const {
        return plane_;
    }
    
    Triangle2 project_to_2d(const Vector3& normal) const {
        // Выбираем ось с максимальным влиянием в нормали
        const double ax = std::fabs(normal.x);
        const double ay = std::fabs(normal.y);
        const double az = std::fabs(normal.z);

        std::array<Point2, 3> result{};

        if (ax > ay && ax > az) {
            // отбросить X → остаются (y, z)
            for (int i = 0; i < 3; ++i) {
                result[i] = Point2(points_[i].y, points_[i].z);
            }
        } else if (ay > ax && ay > az) {
            // отбросить Y → остаются (x, z)
            for (int i = 0; i < 3; ++i) {
                result[i] = Point2(points_[i].x, points_[i].z);
            }
        } else {
            // отбросить Z → остаются (x, y)
            for (int i = 0; i < 3; ++i) {
                result[i] = Point2(points_[i].x, points_[i].y);
            }
        }

        return Triangle2{result[0], result[1], result[2]};
    }

private:
    bool intersects_on_plane_intersection_line(const Triangle3& other_t) const {
        const Vector3 intersection_vec = 
            plane_.get_normal().cross(other_t.get_plane().get_normal());

        auto intersection_proj_other_t = 
            compute_projection_on_plane_intesection_vector(other_t, plane_, intersection_vec);

        auto intersection_proj = 
            compute_projection_on_plane_intesection_vector(
                *this, other_t.get_plane(), intersection_vec
            );

        const bool overlap = 
            std::max(intersection_proj[0], intersection_proj_other_t[0]) <= 
            std::min(intersection_proj[1], intersection_proj_other_t[1]);
        
        return overlap;
    }

    bool intersects_2d(const Triangle3& other_t) const {
        const Triangle2 t1 = project_to_2d(plane_.get_normal());
        const Triangle2 t2 = other_t.project_to_2d(plane_.get_normal());

        return t1.intersects(t2);
    }
    
    static std::array<double, 2> compute_projection_on_plane_intesection_vector(
      const Triangle3& tri, const Plane3& plane, const Vector3& dir) {
        auto distances = distances_to_plane(tri, plane);
        const int uniq_id = find_unique_vertex(distances);

        if (uniq_id == -1) {
            throw std::runtime_error("No unique vertex found: triangle may be coplanar or on one side of the plane");
        }

        std::array<double, 2> projections{};
        int idx = 0;
        for (int i = 0; i < 3; ++i) {
            if (i == uniq_id) continue;

            Vector3 p = intersect_edge_with_plane(
                tri.get_point(i), distances[i],
                tri.get_point(uniq_id), distances[uniq_id]
            );
            projections[idx++] = p.dot(dir);
        }

        if (projections[0] > projections[1]) {
            std::swap(projections[0], projections[1]);
        }

        return projections;
    }

    static std::array<double, 3> distances_to_plane(const Triangle3& tri, const Plane3& plane) {
        return {
            plane.distance_to_point(tri.get_point(0)),
            plane.distance_to_point(tri.get_point(1)),
            plane.distance_to_point(tri.get_point(2))
        };
    }

    static int find_unique_vertex(const std::array<double,3>& distance) {
        int zero_idx   = -1;
        int zero_count = 0;
        int nonzero_idx   = -1;
        int nonzero_count = 0;

        for (int i = 0; i < 3; ++i) {
            if (std::fabs(distance[i]) < EPS) {
                zero_idx = i; zero_count++;
            } else {
                nonzero_idx = i; nonzero_count++;
            }
        }

        if (zero_count == 1) { return zero_idx; }
        if (nonzero_count == 1) { return nonzero_idx; }

        for (int i = 0; i < 3; ++i) {
            int const j = (i + 1) % 3;
            int const k = (i + 2) % 3;
            if (distance[i] * distance[j] < -EPS && 
                distance[i] * distance[k] < -EPS) { return i; }
        }

        return -1;
    }


    static Vector3 intersect_edge_with_plane(const Point3& p1, double d1,
                                      const Point3& p2, double d2) {
        Vector3 edge{p1, p2};
        double t = d1 / (d1 - d2);  // параметр пересечения
        return Vector3{p1} + edge.scale(t);
    }

    Plane3 plane_;
    std::array<Point3, 3> points_;
};

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

export module triangle_intersection;

const double EPS = 1e-12;

namespace geometry {

export struct Point2;
export struct Vector2;
export class  LineSegment2;
export class  Triangle2;

export struct Point3;
export class  Vector3;
export class  Line3;
export class  LineSegment3;
export class  Plane3;
export class  Triangle3;

bool point3_point3_intersects(const Point3& p1, const Point3& p2);
Point2 project_point3_to_point2(const Point3& p, const Vector3& normal);
export double scalar_triple_product(const Vector3& v1, const Vector3& v2, const Vector3& v3);

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
    LineSegment2(const Point2& p1, const Point2& p2) : points_{p1, p2} {}

    enum class Type : std::uint8_t {
        LineSegment = 0,
        Point = 1
    };

    bool is_degenerate() const {
        return std::fabs(points_[0].x - points_[1].x) < EPS &&
               std::fabs(points_[0].y - points_[1].y) < EPS;
    }

    Type get_type() const {
        if (is_degenerate()) { return Type::Point; } 
        return Type::LineSegment;
    }

    bool contains_point(const Point2& r) const {
        const Point2& p = points_[0];
        const Point2& q = points_[1];

        const Vector2 v1{r, p};
        const Vector2 v2{r, q};
        if (std::fabs(v1.cross(v2)) > EPS) { return false; }

        return std::min(p.x, q.x) - EPS < r.x && r.x < std::max(p.x, q.x) + EPS &&
               std::min(p.y, q.y) - EPS < r.y && r.y < std::max(p.y, q.y) + EPS;
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

        if (std::fabs(d1) < EPS && contains_point(c))          { return true; }
        if (std::fabs(d2) < EPS && contains_point(d))          { return true; }
        if (std::fabs(d3) < EPS && other_ls.contains_point(a)) { return true; }
        if (std::fabs(d4) < EPS && other_ls.contains_point(b)) { return true; }
        
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
      : points_{p1, p2, p3} {}

    enum class Type : std::uint8_t {
        Triangle = 0,
        LineSegment = 1,
        Point = 2
    };

    bool is_degenerate() const {
        const Vector2 v1{points_[0], points_[1]};
        const Vector2 v2{points_[0], points_[2]};

        return std::fabs(v1.cross(v2)) < EPS;
    }

    Type get_type() const {
        if (is_degenerate()) { 
            const Vector2 v1{points_[0], points_[1]};
            const Vector2 v2{points_[0], points_[2]};

            if (v1.length() < EPS && v2.length() < EPS) {
                return Type::Point;
            }

            return Type::LineSegment;
        }
        return Type::Triangle; 
    }

    bool contains_point(const Point2& p) const {
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
            const LineSegment2 ls1{points_.at(i), points_.at((i + 1) % 3)};
            for (int j = 0; j < 3; ++j) {
                const LineSegment2 ls2{other_t.get_point(j), other_t.get_point((j + 1) % 3)};
                if (ls1.intersects(ls2)) {
                    return true;
                }
            }
        }

        if (contains_point(other_t.get_point(0)))  { return true; }
        if (other_t.contains_point(points_.at(0))) { return true; }

        return false;
    }

    bool intersects(const LineSegment2& ls) const {
        for (int i = 0; i < 3; ++i) {
            const LineSegment2 ls1{points_.at(i), points_.at((i + 1) % 3)};
            if (ls1.intersects(ls)) { return true; }
        }

        return contains_point(ls.get_point(0));
    }

    const Point2& get_point(int index) const {
        return points_.at(index);
    }

    const auto& get_points() const {
        return points_;
    }

private:
    bool contains_point_point(const Point2& p) const {
        return Vector2{points_[0], p}.length() < EPS;
    }    

    bool contains_point_segment(const Point2& p) const {
        return LineSegment2{points_[0], points_[1]}.contains_point(p) ||
               LineSegment2{points_[0], points_[2]}.contains_point(p);
    }

    bool contains_point_triangle(const Point2& p) const {
        for (const auto& v: points_) {
            if (Vector2{v, p}.length() < EPS) { return true; }
        }
    
        for (int i = 0; i < 3; ++i) {
            if (LineSegment2{points_.at(i), points_.at((i+1)%3)}.contains_point(p)) {
                return true;
            }
        }

        int count = 0;
        for (int i = 0; i < 3; ++i) {
            const Point2& a = points_.at(i);
            const Point2& b = points_.at((i + 1) % 3);

            if (p.y < std::max(a.y, b.y) && p.y > std::min(a.y, b.y)) {
                const double x_int = a.x + ((b.x - a.x) * ((a.y - p.y) / (a.y - b.y)));

                if (p.x < x_int) {
                    ++count;
                }
            }
        }
        return ((count % 2) == 1);
    }

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

class Line3 {
public:
    Line3(const Vector3& v, const Point3& p) : d_(p), dir_(v) {
        if (v.length() < EPS) {
            throw std::invalid_argument("Degenerate direction");
        }
    }

    std::optional<Point3> intersects(const Line3& other_l) const {
        const Vector3 d1d2{d_, other_l.get_d()};

        if (std::fabs(scalar_triple_product(dir_, other_l.get_dir(), d1d2)) > EPS) {
            return std::nullopt;
        }

        if (dir_.cross(other_l.get_dir()).length() < EPS) {
            if (dir_.cross(d1d2).length() > EPS) {
                return std::nullopt;
            }
            return d_;
        }

        Vector3 u = dir_;
        const Vector3 v = other_l.get_dir();
        const Point3 p1 = d_;
        const Point3 p2 = other_l.get_d();

        const Vector3 n = u.cross(v);
        const Vector3 w{p1, p2};

        const double t = w.cross(v).dot(n) / n.dot(n);

        const Vector3 res = Vector3{p1} + u.scale(t);

        return Point3{.x=res.x, .y=res.y, .z=res.z};
    }

    const Point3& get_d() const {
        return d_;
    }

    const Vector3& get_dir() const {
        return dir_;
    }

private:
    Point3 d_;
    Vector3 dir_;
};

class LineSegment3 {
    public:
    LineSegment3(const Point3& p1, const Point3& p2) : points_{p1, p2} {}

    enum class Type : std::uint8_t {
        LineSegment = 0,
        Point = 1
    };

    bool is_degenerate() const {
        return std::fabs(points_[0].x - points_[1].x) < EPS &&
               std::fabs(points_[0].y - points_[1].y) < EPS &&
               std::fabs(points_[0].z - points_[1].z) < EPS;
    }

    Type get_type() const {
        if (is_degenerate()) { return Type::Point; } 
        return Type::LineSegment;
    }

    bool contains_point(const Point3& r) const {
        const Point3& p = points_[0];
        const Point3& q = points_[1];

        const Vector3 v1{r, p};
        const Vector3 v2{r, q};
        if (v1.cross(v2).length() > EPS) { return false; }

        return is_point_in_box(r);
    }

    bool intersects(const LineSegment3& other_ls) const {
        if (is_degenerate() || other_ls.is_degenerate()) {
            return degenerate_intersects(other_ls);
        }
        const Line3 l1{
            Vector3{points_[0], points_[1]}, 
            points_[0]
        };
        
        const Line3 l2{
            Vector3{other_ls.get_point(0), other_ls.get_point(1)}, 
            other_ls.get_point(0)
        };

        auto intersect = l1.intersects(l2);
        if (intersect) {
            return is_point_in_box(intersect.value()) &&
                   other_ls.is_point_in_box(intersect.value());
        }
        
        return false;
    }
    
    bool is_point_in_box(const Point3& p) const {
        auto in_range = [](double v, double a, double b) {
            return std::min(a, b) - EPS < v && v < std::max(a, b) + EPS;
        };

        return in_range(p.x, points_[0].x, points_[1].x) &&
               in_range(p.y, points_[0].y, points_[1].y) &&
               in_range(p.z, points_[0].z, points_[1].z);
    }

    LineSegment2 project_to_2d(const Vector3& normal) const {
        // Выбираем ось с максимальным влиянием в нормали
        const double ax = std::fabs(normal.x);
        const double ay = std::fabs(normal.y);
        const double az = std::fabs(normal.z);

        std::array<Point2, 2> projections;
        if (ax > ay && ax > az) {
            // отбросить X → остаются (y, z)
            projections[0] = Point2{points_[0].y, points_[0].z};
            projections[1] = Point2{points_[1].y, points_[1].z};
        } else if (ay > ax && ay > az) {
            // отбросить Y → остаются (x, z)
            projections[0] = Point2{points_[0].x, points_[0].z};
            projections[1] = Point2{points_[1].x, points_[1].z};
        } else {
            // отбросить Z → остаются (x, y)
            projections[0] = Point2{points_[0].x, points_[0].y};
            projections[1] = Point2{points_[1].x, points_[1].y};
        }

        return LineSegment2{projections[0], projections[1]};
    } 

    double length() const {
        return Vector3{points_[0], points_[1]}.length();
    }

    const Point3& get_point(int index) const {
        return points_.at(index);
    }

    const auto& get_points() const {
        return points_;
    }
    
private:
    bool degenerate_intersects(const LineSegment3& other_ls) const {
        if (get_type() == Type::LineSegment && other_ls.get_type() == Type::Point) {
            return contains_point(other_ls.get_point(0));
        }
        if (get_type() == Type::Point && other_ls.get_type() == Type::LineSegment) {
            return other_ls.contains_point(points_[0]);            
        }
        return point3_point3_intersects(points_[0], other_ls.get_point(0));
    }

    std::array<Point3, 2> points_;
};

class Plane3 {
public:
    Plane3(Point3 p1, Point3 p2, Point3 p3) {
        Vector3 vec1{p1, p2};
        Vector3 vec2{p1, p3};

        normal = vec1.cross(vec2);
        d = - normal.dot(Vector3{p1});
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
      : plane_(p1, p2, p3), points_{p1, p2, p3} {}

    enum class Type : std::uint8_t {
        Triangle    = 0,
        LineSegment = 1,
        Point       = 2
    };

    bool is_valid() const {
        Vector3 v1{points_[0], points_[1]};
        Vector3 v2{points_[0], points_[2]};
        return v1.cross(v2).length() > EPS;
    }

    bool is_degenerate() const {
        return !is_valid();
    }

    Type get_type() const {
        if (is_degenerate()) {
            const Vector3 v1{points_[0], points_[1]};
            const Vector3 v2{points_[0], points_[2]};

            if (v1.length() < EPS && v2.length() < EPS) { return Type::Point; }

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

    bool intersects(const Plane3& plane) const {
        if (!plane.is_valid()) {
             throw std::invalid_argument("Degenerate plane");
        }

        const auto distances = distances_to_plane(*this, plane);

        const auto [min, max] = std::ranges::minmax_element(distances);

        const bool is_intersect = *min < -EPS && *max > EPS; 

        const bool is_lie = std::ranges::any_of(
            distances, 
            [](double d){ return std::fabs(d) < EPS; }
        );

        return  is_intersect || is_lie;
    }

    bool intersects(const LineSegment3& ls) const {
        const double d1 = plane_.distance_to_point(ls.get_point(0));
        const double d2 = plane_.distance_to_point(ls.get_point(1));

        if (d1 * d2 < -EPS) {
            const Vector3 intersection_vec = 
                Vector3{ls.get_point(0)} + 
                Vector3{
                    ls.get_point(0), 
                    ls.get_point(1)
                }.scale(std::fabs(d1)/(std::fabs(d1) + std::fabs(d2)));
            
            return intersects(Point3{.x=intersection_vec.x, .y=intersection_vec.y, .z=intersection_vec.z});
        }
        if (std::fabs(d1) < EPS && std::fabs(d2) < EPS) {
            const Triangle2    tri2 = project_to_2d(plane_.get_normal());
            const LineSegment2 ls2  = ls.project_to_2d(plane_.get_normal());

            return tri2.intersects(ls2);
        }
        return false;
    }

    bool intersects(const Point3& p) const {
        if (plane_.distance_to_point(p) > EPS) {
            return false;
        }
        const Vector3 normal = plane_.get_normal();
        const Triangle2 tri2 = project_to_2d(normal);

        const Point2 p2 = project_point3_to_point2(p, normal);

        return tri2.contains_point(p2);
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
                result.at(i) = Point2(points_.at(i).y, points_.at(i).z);
            }
        } else if (ay > ax && ay > az) {
            // отбросить Y → остаются (x, z)
            for (int i = 0; i < 3; ++i) {
                result.at(i) = Point2(points_.at(i).x, points_.at(i).z);
            }
        } else {
            // отбросить Z → остаются (x, y)
            for (int i = 0; i < 3; ++i) {
                result.at(i) = Point2(points_.at(i).x, points_.at(i).y);
            }
        }

        return Triangle2{result[0], result[1], result[2]};
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
                return point3_point3_intersects(points_[0], other_t.get_point(0));
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

    LineSegment3 degenerate_segment() const {
        LineSegment3 max_ls{Point3{.x=0, .y=0 ,.z=0}, Point3{.x=0, .y=0, .z=0}};
        double max_length = 0;
        for (int i = 0; i < 3; ++i) {
            const LineSegment3 ls{points_.at(i), points_.at((i+i)%3)};
            const double ls_length = ls.length();
            if (ls_length > max_length) {
                max_ls = ls;
                max_length = ls_length;
            }
        }
        return max_ls;
    }

    bool intersects_on_plane_intersection_line(const Triangle3& other_t) const {
        const Vector3 intersection_vec = 
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
        const Triangle2 t1 = project_to_2d(plane_.get_normal());
        const Triangle2 t2 = other_t.project_to_2d(plane_.get_normal());

        return t1.intersects(t2);
    }
    
    static std::array<double, 2> compute_projection_on_plane_intesection_vector(
        const Triangle3& tri, const Plane3& plane, const Vector3& dir) 
    {
        auto distances = distances_to_plane(tri, plane);

        auto zeroes_indices = std::ranges::views::iota(0, 3)
            | std::ranges::views::filter([&](int i){ return std::fabs(distances.at(i)) < EPS; });

        const std::vector<int> zi{zeroes_indices.begin(), zeroes_indices.end()};

        std::array<double, 2> projections{};

        if (zi.size() == 2) {
            projections[0] = dir.dot(tri.get_point(zi[0]));
            projections[1] = dir.dot(tri.get_point(zi[1]));
        }

        if (zi.size() == 1) {
            const int i = zi[0];
            if (distances.at((i+1)%3) * distances.at((i+2)%3) < -EPS) {
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
                if (distances.at(i) * distances.at(j) < -EPS && 
                    distances.at(i) * distances.at(k) < -EPS) {
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

    static std::array<double, 3> distances_to_plane(const Triangle3& tri, const Plane3& plane) {
        return {
            plane.distance_to_point(tri.get_point(0)),
            plane.distance_to_point(tri.get_point(1)),
            plane.distance_to_point(tri.get_point(2))
        };
    }

    static Vector3 intersect_edge_with_plane(const Point3& p1, double d1,
                                             const Point3& p2, double d2) 
    {
        Vector3 edge{p1, p2};
        const double t = d1 / (d1 - d2);  // параметр пересечения
        return Vector3{p1} + edge.scale(t);
    }

    Plane3 plane_;
    std::array<Point3, 3> points_;
};

Point2 project_point3_to_point2(const Point3& p, const Vector3& normal) {
    // Выбираем ось с максимальным влиянием в нормали
    const double ax = std::fabs(normal.x);
    const double ay = std::fabs(normal.y);
    const double az = std::fabs(normal.z);

    if (ax > ay && ax > az) {
        // отбросить X → остаются (y, z)
        return Point2{p.y, p.z};
    } else if (ay > ax && ay > az) {
        // отбросить Y → остаются (x, z)
        return Point2{p.x, p.z};
    } else {
        // отбросить Z → остаются (x, y)
        return Point2{p.x, p.y};
    }
}

bool point3_point3_intersects(const Point3& p1, const Point3& p2) {
    return Vector3{p1, p2}.length() < EPS;
}

 double scalar_triple_product(const Vector3& v1, const Vector3& v2, const Vector3& v3) {
    return (v1.x * (v2.y * v3.z - v2.z * v3.y)) -
           (v1.y * (v2.x * v3.z - v2.z * v3.x)) +
           (v1.z * (v2.x * v3.y - v2.y * v3.x));
 }

} // namespace geometry
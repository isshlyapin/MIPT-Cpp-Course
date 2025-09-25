module;

#include <array>
#include <cmath>
#include <utility>
#include <cassert>

export module triangle_intersection;

const double EPS = 1e-6;

export struct Point3 {
    double x{0}, y{0}, z{0};
};

struct Vector3 {
    Vector3(const Point3& p1, const Point3& p2) 
      : x(p2.x - p1.x), y(p2.y - p1.y), z(p2.z - p1.z) {}
    
    Vector3(double x = 0, double y = 0, double z = 0)
      : x(x), y(y), z(z) {}
    
    Vector3(const Point3& p) : Vector3(p.x, p.y, p.z) {}

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
    
    double x{0}, y{0}, z{0};
};

struct Plane3 {
    Plane3(Point3 p1, Point3 p2, Point3 p3) {
        Vector3 vec1{p1, p2};
        Vector3 vec2{p1, p3};

        normal = vec1.cross(vec2);
        d = - normal.dot(Vector3{p1});
    }

    double distance_to_point(const Point3& p) const {
        return normal.dot(p) + d;
    }

    double d{0};
    Vector3 normal;
};

struct LineSegment3 {
    LineSegment3(const Point3& p1, const Point3& p2) : p1(p1), p2(p2) {}

    LineSegment3(const Vector3& v1, const Vector3& v2) 
      : LineSegment3(Point3{.x=v1.x, .y=v1.y, .z=v1.z}, Point3{.x=v2.x, .y=v2.y, .z=v2.z}) {}

    Point3 p1, p2;
};

struct Point2 {
    double x{0}, y{0};
};

class Triangle2 {
public:
    Triangle2(const Point2& p1, const Point2& p2, const Point2& p3)
      : points_{p1, p2, p3} {}

    double square() const {
        return 0.5 * ((points_[1].x - points_[0].x) * (points_[2].y - points_[0].y) -
                      (points_[2].x - points_[0].x) * (points_[1].y - points_[0].y));
    }

    bool intersects(const Triangle2& other_t) const {
        // Проверка пересечения двух треугольников в 2D
        auto point_in_triangle = [](const Point2& p, const std::array<Point2, 3>& tri) {
            double d1 = (p.x - tri[1].x) * (tri[0].y - tri[1].y) - (tri[0].x - tri[1].x) * (p.y - tri[1].y);
            double d2 = (p.x - tri[2].x) * (tri[1].y - tri[2].y) - (tri[1].x - tri[2].x) * (p.y - tri[2].y);
            double d3 = (p.x - tri[0].x) * (tri[2].y - tri[0].y) - (tri[2].x - tri[0].x) * (p.y - tri[0].y);
            return ((d1 >= -EPS && d2 >= -EPS && d3 >= -EPS) || (d1 <= EPS && d2 <= EPS && d3 <= EPS));
        };

        auto segments_intersect = [](const Point2& a, const Point2& b, const Point2& c, const Point2& d) {
            auto cross = [](const Point2& p1, const Point2& p2, const Point2& p3) {
                return (p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x);
            };
            double d1 = cross(a, b, c);
            double d2 = cross(a, b, d);
            double d3 = cross(c, d, a);
            double d4 = cross(c, d, b);

            if (((d1 > EPS && d2 < -EPS) || (d1 < -EPS && d2 > EPS)) &&
                ((d3 > EPS && d4 < -EPS) || (d3 < -EPS && d4 > EPS))) {
                return true;
            }
            // Проверка на совпадение концов
            auto on_segment = [](const Point2& p, const Point2& q, const Point2& r) {
                return std::min(p.x, q.x) - EPS <= r.x && r.x <= std::max(p.x, q.x) + EPS &&
                       std::min(p.y, q.y) - EPS <= r.y && r.y <= std::max(p.y, q.y) + EPS;
            };
            if (std::fabs(d1) < EPS && on_segment(a, b, c)) return true;
            if (std::fabs(d2) < EPS && on_segment(a, b, d)) return true;
            if (std::fabs(d3) < EPS && on_segment(c, d, a)) return true;
            if (std::fabs(d4) < EPS && on_segment(c, d, b)) return true;
            return false;
        };

        const auto& tri1 = points_;
        const auto& tri2 = other_t.points_;

        for (int i = 0; i < 3; ++i) {
            if (point_in_triangle(tri1[i], tri2)) return true;
            if (point_in_triangle(tri2[i], tri1)) return true;
        }

        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                if (segments_intersect(tri1[i], tri1[(i+1)%3], tri2[j], tri2[(j+1)%3])) {
                    return true;
                }
            }
        }
        return false;
    }

private:
    std::array<Point2, 3> points_;
};

export class Triangle3 {
public:
    Triangle3(const Point3& p1, const Point3& p2, const Point3& p3)
      : plane_(p1, p2, p3), points_{p1, p2, p3} {}
    
    bool intersects(const Triangle3& other_t) const {
        const std::array<double, 3> distances_other_t{
            plane_.distance_to_point(other_t.get_vertex(0)),
            plane_.distance_to_point(other_t.get_vertex(1)),
            plane_.distance_to_point(other_t.get_vertex(2))
        };

        // Triangles from the same plane
        if (std::fabs(distances_other_t[0]) < EPS &&
            std::fabs(distances_other_t[1]) < EPS && 
            std::fabs(distances_other_t[2]) < EPS) {
            return intersects_2d(other_t);
        }

        const std::array<int, 3> distance_signs_other_t{
            sign(distances_other_t[0]),
            sign(distances_other_t[1]),
            sign(distances_other_t[2])
        };

        const std::array<double, 3> distances{
            other_t.get_plane().distance_to_point(points_[0]),
            other_t.get_plane().distance_to_point(points_[1]),
            other_t.get_plane().distance_to_point(points_[2])
        };
       
        const std::array<int, 3> distance_signs{
            sign(distances[0]),
            sign(distances[1]),
            sign(distances[2])
        };
        
        const int uniq_id_other_t = unique_index(distance_signs_other_t);
        const int uniq_id = unique_index(distance_signs);

        if (uniq_id_other_t != -1 && uniq_id != -1) {
            return handle_plane_intersection(other_t, distances_other_t, distances, uniq_id_other_t, uniq_id);
        }
        
        return false;
    }

    const Point3& get_vertex(int index) const {
        return points_.at(index);
    }

    const Plane3& get_plane() const {
        return plane_;
    }
    
private:
    bool handle_plane_intersection(
      const Triangle3& other_t, 
      const std::array<double, 3>& distances_other_t,
      const std::array<double, 3>& distances, 
      int uniq_id_other_t,
      int uniq_id) const {
        auto compute_projection = [](
            const Triangle3& tri,
            const Vector3 dir,
            const std::array<double, 3>& distances,
            int uniq_id
            ) {
            std::array<double, 2> points{};
            int idx = 0;

            for (int i = 0; i < 3; ++i) {
                if (i == uniq_id) continue;

                Vector3 edge = Vector3{tri.get_vertex(i), tri.get_vertex(uniq_id)};
                double t = distances[i] / (distances[i] + distances[uniq_id]);
                points[idx++] = (Vector3{tri.get_vertex(i)} + edge.scale(t)).dot(dir);
            }
            return points;
        };

        const Vector3 intersection_dir = plane_.normal.cross(other_t.get_plane().normal);

        auto intersection_projections_other_t = 
            compute_projection(other_t, intersection_dir, distances_other_t, uniq_id_other_t);

        auto intersection_projections = 
            compute_projection(*this, intersection_dir, distances, uniq_id);
            
        if (intersection_projections[0] > intersection_projections[1]) {
            std::swap(intersection_projections[0], intersection_projections[1]);
        }

        if (intersection_projections_other_t[0] > intersection_projections_other_t[1]) {
            std::swap(intersection_projections_other_t[0], intersection_projections_other_t[1]);
        }

        const bool overlap = 
            std::max(intersection_projections[0], intersection_projections_other_t[0]) <= 
            std::min(intersection_projections[1], intersection_projections_other_t[1]);
        
        return overlap;
    }

    bool intersects_2d(const Triangle3& other_t) const {
        auto proj1 = project_to_2d(points_, plane_.normal);
        auto proj2 = project_to_2d(
            {other_t.get_vertex(0), other_t.get_vertex(1), other_t.get_vertex(2)},
            plane_.normal
        );

        const Triangle2 t1{proj1[0], proj1[1], proj1[2]};
        const Triangle2 t2{proj2[0], proj2[1], proj2[2]};

        return t1.intersects(t2);
    }

    static std::array<Point2, 3> project_to_2d(
    const std::array<Point3, 3>& pts,
    const Vector3& normal) {
        // Выбираем ось с максимальным влиянием в нормали
        const double ax = std::fabs(normal.x);
        const double ay = std::fabs(normal.y);
        const double az = std::fabs(normal.z);

        std::array<Point2, 3> result{};

        if (ax > ay && ax > az) {
            // отбросить X → остаются (y, z)
            for (int i = 0; i < 3; i++) {
                result[i] = {.x=pts[i].y, .y=pts[i].z};
            }
        } else if (ay > az) {
            // отбросить Y → остаются (x, z)
            for (int i = 0; i < 3; i++) {
                result[i] = {.x=pts[i].x, .y=pts[i].z};
            }
        } else {
            // отбросить Z → остаются (x, y)
            for (int i = 0; i < 3; i++) {
                result[i] = {.x=pts[i].x, .y=pts[i].y};
            }
        }

        return result;
    }


    int unique_index(const std::array<int, 3>& s) const {
        if (s[0] == s[1] && s[2] != s[0]) return 2;
        if (s[0] == s[2] && s[1] != s[0]) return 1;
        if (s[1] == s[2] && s[0] != s[1]) return 0;
        
        return -1;
    }

    static int sign(double x) {
        if (x > EPS) {
            return 1;
        }

        if (x < -EPS) {
            return -1;            
        }

        return 0;
    }

    Plane3 plane_;
    std::array<Point3, 3> points_;
};
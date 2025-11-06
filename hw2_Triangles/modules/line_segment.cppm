module;

#include <cmath>
#include <array>
#include <cstdint>
#include <algorithm>

import isshlyapin.line;
import isshlyapin.point;
import isshlyapin.vector;
import isshlyapin.config;

export module isshlyapin.line_segment;

namespace geometry {

export class LineSegment2 {
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

export class LineSegment3 {
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
        return points_[0] == other_ls.get_point(0);
    }

    std::array<Point3, 2> points_;
};

} // namespace geometry
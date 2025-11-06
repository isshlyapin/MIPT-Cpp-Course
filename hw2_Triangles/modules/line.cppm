module;

#include <cmath>
#include <optional>
#include <stdexcept>

import isshlyapin.point;
import isshlyapin.vector;
import isshlyapin.config;

export module isshlyapin.line;

namespace geometry {

double scalar_triple_product(const Vector3& v1, const Vector3& v2, const Vector3& v3) {
    return (v1.x * (v2.y * v3.z - v2.z * v3.y)) -
           (v1.y * (v2.x * v3.z - v2.z * v3.x)) +
           (v1.z * (v2.x * v3.y - v2.y * v3.x));
}

export class Line3 {
public:
    Line3(const Vector3& v, const Point3& p) : d_(p), dir_(v) {
        if (v.length() < EPS) {
            throw std::invalid_argument("Uncorrect direction");
        }
    }

    std::optional<Point3> intersects(const Line3& other_l) const {
        const Vector3 d1d2{d_, other_l.get_point()};

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
        const Point3 p2 = other_l.get_point();

        const Vector3 n = u.cross(v);
        const Vector3 w{p1, p2};

        const double t = w.cross(v).dot(n) / n.dot(n);

        const Vector3 res = Vector3{p1} + u.scale(t);

        return Point3{res.x, res.y, res.z};
    }

    const Point3& get_point() const {
        return d_;
    }

    const Vector3& get_dir() const {
        return dir_;
    }

private:
    Point3 d_;
    Vector3 dir_;
};

} //namespace geometry

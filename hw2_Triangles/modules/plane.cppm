module;

#include <stdexcept>

import isshlyapin.point;
import isshlyapin.vector;
import isshlyapin.config;

export module isshlyapin.plane;

namespace geometry {

export class Plane3 {
public:
    Plane3(Point3 p1, Point3 p2, Point3 p3) {
        Vector3 vec1{p1, p2};
        Vector3 vec2{p1, p3};

        normal = vec1.cross(vec2);
        d = - normal.dot(Vector3{p1});

        // if (!is_valid()) { throw std::invalid_argument("Uncorrect points"); }
    }

    Plane3(const Vector3& v, double d = 0) : normal{v}, d{d} {
        // if (!is_valid()) { throw std::invalid_argument("Uncorrect points"); }
    }

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

} // namespace geometry
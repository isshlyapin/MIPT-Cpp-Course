module;

#include <cmath>

import isshlyapin.config;

export module isshlyapin.point;

namespace geometry {

export struct Point2 {
    Point2(double x = 0, double y = 0) : x(x), y(y) {}

    double x, y;
};

export struct Point3 {
    Point3(double x = 0, double y = 0, double z = 0) : x(x), y(y), z(z) {}

    bool operator== (const Point3& other) const {
        double dist = std::sqrt(
            (x - other.x) * (x - other.x) +
            (y - other.y) * (y - other.y) +
            (z - other.z) * (z - other.z)
        );

        return dist < EPS;
    }

    double x, y, z;
};

} // namespace geometry
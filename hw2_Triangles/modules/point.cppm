module;

#include <cmath>
#include <concepts>

export module isshlyapin.point;

import isshlyapin.config;

namespace geometry {

export
template<std::floating_point T>
struct Point2 {
    Point2(T x = T{}, T y = T{}) : x(x), y(y) {}

    T x, y;
};

export 
template<std::floating_point T>
struct Point3 {
    Point3(T x = T{}, T y = T{}, T z = T{}) : x(x), y(y), z(z) {}

    bool operator== (const Point3& other) const {
        T dist = std::sqrt(
            (x - other.x) * (x - other.x) +
            (y - other.y) * (y - other.y) +
            (z - other.z) * (z - other.z)
        );

        return dist < EPS<T>;
    }

    T x, y, z;
};

} // namespace geometry
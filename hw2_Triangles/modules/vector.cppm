module;

#include <cmath>
#include <concepts>

export module isshlyapin.vector;

import isshlyapin.point;

namespace geometry {

export 
template<std::floating_point T>
struct Vector2 {
    Vector2(T x = T{}, T y = T{}) : x(x), y(y) {}
    
    Vector2(const Point2<T>& p1, const Point2<T>& p2) : Vector2(p2.x - p1.x, p2.y - p1.y) {}
    
    Vector2(const Point2<T>& p) : Vector2(p.x, p.y) {}

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

    T x, y;
};

export 
template<std::floating_point T>
struct Vector3 {
    Vector3(T x = T{}, T y = T{}, T z = T{}) : x(x), y(y), z(z) {}

    Vector3(const Point3<T>& p) : Vector3(p.x, p.y, p.z) {}

    Vector3(const Point3<T>& p1, const Point3<T>& p2) 
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
    
    T x, y, z;
};

} // namespace geometry
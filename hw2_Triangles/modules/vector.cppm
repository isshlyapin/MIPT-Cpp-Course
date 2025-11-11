module;

#include <cmath>

export module isshlyapin.vector;

import isshlyapin.point;

namespace geometry {

export struct Vector2 {
    Vector2(double x = 0, double y = 0) : x(x), y(y) {}
    
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

export struct Vector3 {
    Vector3(double x = 0, double y = 0, double z = 0) : x(x), y(y), z(z) {}

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

} // namespace geometry
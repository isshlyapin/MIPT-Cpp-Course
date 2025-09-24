#include <array>
module;


export module triangle_intersection;

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

    double dot(const Vector3& other_v) const {
        return (x * other_v.x) + (y * other_v.y) + (z * other_v.z);
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
        return (normal.x * p.x) + (normal.y * p.y) + (normal.z * p.z) + d;
    }

    double d{0};
    Vector3 normal;
};

export class Triangle3 {
    Triangle3(const Point3& p1, const Point3& p2, const Point3& p3)
      : plane_(p1, p2, p3), points_{p1, p2, p3} {}
    
    bool intersects(const Triangle3& other_t) const {
        
    }

    const Point3& get_vertex(int index) const {
        return points_.at(index);
    }
private:
    Plane3 plane_;
    std::array<Point3, 3> points_;
};
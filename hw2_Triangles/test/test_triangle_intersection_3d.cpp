#include <cmath>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

import triangle_intersection;

using Catch::Approx;
using namespace geometry;

TEST_CASE("Point2 and Vector2 basic operations", "[Point2][Vector2]") {
    Point2 p1{1.0, 2.0};
    Point2 p2{4.0, 6.0};
    Vector2 v1{p1, p2};
    REQUIRE(v1.x == Approx(3.0));
    REQUIRE(v1.y == Approx(4.0));
    REQUIRE(v1.length() == Approx(5.0));
    REQUIRE(v1.length_square() == Approx(25.0));
    Vector2 v2{2.0, 3.0};
    REQUIRE(v1.dot(v2) == Approx(18.0));
    REQUIRE(v1.cross(v2) == Approx(1.0));
}

TEST_CASE("Plane3 construction and validity", "[Plane3]") {
    Point3 p1{0, 0, 0};
    Point3 p2{1, 0, 0};
    Point3 p3{0, 1, 0};
    Plane3 plane{p1, p2, p3};
    REQUIRE(plane.is_valid());

    // Degenerate plane: all points equal
    Plane3 plane_equal{{0,0,0}, {0,0,0}, {0,0,0}};
    REQUIRE_FALSE(plane_equal.is_valid());
}

TEST_CASE("Plane3 normal and distance", "[Plane3]") {
    Point3 p1{0, 0, 0};
    Point3 p2{1, 0, 0};
    Point3 p3{0, 1, 0};
    Plane3 plane{p1, p2, p3};

    Vector3 normal = plane.get_normal();
    // Normal should be (0,0,1) or (0,0,-1) depending on orientation
    REQUIRE((Approx(std::fabs(normal.x)) == 0.0));
    REQUIRE((Approx(std::fabs(normal.y)) == 0.0));
    REQUIRE((Approx(std::fabs(normal.z)) == 1.0));

    // Point on plane
    REQUIRE(plane.distance_to_point({0, 0, 0}) == Approx(0.0));
    REQUIRE(plane.distance_to_point({1, 0, 0}) == Approx(0.0));
    REQUIRE(plane.distance_to_point({0, 1, 0}) == Approx(0.0));

    // Point above plane
    REQUIRE(plane.distance_to_point({0, 0, 1}) * normal.z > 0);

    // Point below plane
    REQUIRE(plane.distance_to_point({0, 0, -1}) * normal.z < 0);
}

TEST_CASE("Plane3 constructed from normal and d", "[Plane3]") {
    Vector3 n{0, 0, 1};
    double d = -2;
    Plane3 plane{n, d};

    REQUIRE(plane.is_valid());
    REQUIRE(plane.get_normal().x == Approx(0.0));
    REQUIRE(plane.get_normal().y == Approx(0.0));
    REQUIRE(plane.get_normal().z == Approx(1.0));

    // Plane equation: z + d = 0 → z = 2
    REQUIRE(plane.distance_to_point({0, 0, 2}) == Approx(0.0));
    REQUIRE(plane.distance_to_point({0, 0, 3}) == Approx(1.0));
    REQUIRE(plane.distance_to_point({0, 0, 1}) == Approx(-1.0));
}

TEST_CASE("Triangle3 coplanar intersection", "[Triangle3]") {
    Triangle3 t1{{0, 0, 0}, {2, 0, 0}, {1, 2, 0}};
    Triangle3 t2{{1, 1, 0}, {3, 1, 0}, {2, 3, 0}};
    REQUIRE(t1.intersects(t2));

    Triangle3 t3{{3, 3, 0}, {4, 3, 0}, {3.5, 4, 0}};
    REQUIRE_FALSE(t1.intersects(t3));
}

TEST_CASE("Triangle3 non-coplanar intersection", "[Triangle3]") {
    Triangle3 t1{{0, 0, 0}, {2, 0, 0}, {1, 2, 0}};
    Triangle3 t2{{1, 1, -1}, {1, 1, 1}, {2, 3, 0}};
    REQUIRE(t1.intersects(t2));
}

TEST_CASE("Triangle3 no intersection (separated planes)", "[Triangle3]") {
    Triangle3 t1{{0, 0, 0}, {2, 0, 0}, {1, 2, 0}};
    Triangle3 t2{{0, 0, 2}, {2, 0, 2}, {1, 2, 2}};
    REQUIRE_FALSE(t1.intersects(t2));
}

TEST_CASE("Triangle3 degenerate and invalid cases", "[Triangle3]") {
    Triangle3 tri3_degenerate({0,0,0}, {0,0,0}, {0,0,0});
    REQUIRE_FALSE(tri3_degenerate.is_valid());
    REQUIRE(tri3_degenerate.get_type() == Triangle3::Type::Point);

    Triangle3 tri3_degenerate2({0,0,0}, {1,1,1}, {2,2,2});
    REQUIRE_FALSE(tri3_degenerate2.is_valid());
    REQUIRE(tri3_degenerate2.get_type() == Triangle3::Type::LineSegment);

    Triangle3 tri3_degenerate3({0,0,0}, {0,0,0}, {2,2,2});
    REQUIRE_FALSE(tri3_degenerate3.is_valid());
    REQUIRE(tri3_degenerate3.get_type() == Triangle3::Type::LineSegment);
}

TEST_CASE("Triangle3 projection to 2D", "[Triangle3]") {
    Triangle3 t{{0,0,0}, {1,0,0}, {0,1,0}};
    auto t2d = t.project_to_2d({0,0,1});
    REQUIRE(t2d.get_point(0).x == Approx(0.0));
    REQUIRE(t2d.get_point(0).y == Approx(0.0));
    REQUIRE(t2d.get_point(1).x == Approx(1.0));
    REQUIRE(t2d.get_point(1).y == Approx(0.0));
    REQUIRE(t2d.get_point(2).x == Approx(0.0));
    REQUIRE(t2d.get_point(2).y == Approx(1.0));
}

TEST_CASE("Triangle3 degenerate intersections", "[Triangle3]") {
    Triangle3 tri_point({0,0,0}, {0,0,0}, {0,0,0});
    REQUIRE(tri_point.intersects(tri_point));

    Triangle3 tri_point2({1,1,1}, {1,1,1}, {1,1,1});
    REQUIRE_FALSE(tri_point2.intersects(tri_point));

    Triangle3 tri1{{0,0,2}, {0,0,-7}, {1,5,0}};
    REQUIRE(tri1.intersects(tri_point));
    REQUIRE_FALSE(tri1.intersects(tri_point2));

    Triangle3 tri_segment({0,0,0}, {0,0,0}, {2,2,2});
    REQUIRE(tri_segment.intersects(tri_segment));
    REQUIRE(tri_segment.intersects(tri_point));

    Triangle3 tri_segment2({0,0,0}, {0,0,0}, {-2,-2,-2});
    REQUIRE(tri_segment2.intersects(tri_segment));
    REQUIRE_FALSE(tri_segment2.intersects(tri_point2));

    Triangle3 tri_segment3({0,0,-1}, {0,0,-1}, {-2,-2,-2});
    REQUIRE_FALSE(tri_segment3.intersects(tri_segment));

    Triangle3 tri_segment4({5,1,1}, {5,1,1}, {-5,1,1});
    REQUIRE(tri_segment4.intersects(tri_segment));

    Triangle3 tri2{{0,7,0}, {-1,0,0}, {0,0,10}};
    REQUIRE(tri2.intersects(tri_segment4));
}
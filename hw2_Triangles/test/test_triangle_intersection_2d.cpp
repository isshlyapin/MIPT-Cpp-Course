#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

import triangle_intersection;

using Catch::Approx;
using namespace geometry;

TEST_CASE("Basic functionality Triangle2", "[Triangle2]") {
    const Point2 p1{0.0, 0.0};
    const Point2 p2{1.0, 0.0};
    const Point2 p3{0.0, 1.0};

    const Triangle2 t1{p1, p2, p3};
    REQUIRE_FALSE(t1.is_degenerate());
    REQUIRE(t1.get_type() == Triangle2::Type::Triangle);

    REQUIRE(t1.get_point(0).x == Approx(0.0));
    REQUIRE(t1.get_point(1).x == Approx(1.0));
    REQUIRE(t1.get_point(2).y == Approx(1.0));
}

TEST_CASE("Degenerate Triangle2 types", "[Triangle2]") {
    const Triangle2 t_point{Point2{0, 0}, Point2{0, 0}, Point2{0, 0}};
    REQUIRE(t_point.is_degenerate());
    REQUIRE(t_point.get_type() == Triangle2::Type::Point);

    const Triangle2 t_segment{Point2{0, 0}, Point2{1, 1}, Point2{2, 2}};
    REQUIRE(t_segment.is_degenerate());
    REQUIRE(t_segment.get_type() == Triangle2::Type::LineSegment);
}

TEST_CASE("Triangle2 contains_point", "[Triangle2]") {
    const Triangle2 t{Point2{0, 0}, Point2{2, 0}, Point2{0, 2}};

    SECTION("Inside") {
        REQUIRE(t.contains_point(Point2{0.5, 0.5}));
    }

    SECTION("On edge") {
        REQUIRE(t.contains_point(Point2{1, 0}));
        REQUIRE(t.contains_point(Point2{0, 1}));
        REQUIRE(t.contains_point(Point2{1, 1}));
    }

    SECTION("Outside") {
        REQUIRE_FALSE(t.contains_point(Point2{2, 2}));
    }
}

TEST_CASE("Triangle2 contains_point degenerate", "[Triangle2]") {
    const Triangle2 t_point{Point2{1, 1}, Point2{1, 1}, Point2{1, 1}};
    REQUIRE(t_point.contains_point(Point2{1, 1}));
    REQUIRE_FALSE(t_point.contains_point(Point2{2, 2}));

    const Triangle2 t_segment{Point2{0, 0}, Point2{1, 1}, Point2{2, 2}};
    REQUIRE(t_segment.contains_point(Point2{1, 1}));
    REQUIRE_FALSE(t_segment.contains_point(Point2{1, 2}));
}

TEST_CASE("Triangle2 intersection - regular cases", "[Triangle2]") {
    const Triangle2 t1{Point2{0, 0}, Point2{2, 0}, Point2{0, 2}};
    const Triangle2 t2{Point2{1, 1}, Point2{3, 1}, Point2{1, 3}};
    const Triangle2 t3{Point2{3, 3}, Point2{4, 3}, Point2{3, 4}};

    SECTION("Overlapping triangles") {
        REQUIRE(t1.intersects(t2));
        REQUIRE(t2.intersects(t1));
    }

    SECTION("Disjoint triangles") {
        REQUIRE_FALSE(t1.intersects(t3));
        REQUIRE_FALSE(t3.intersects(t1));
    }

    SECTION("One inside another") {
        const Triangle2 t_small{Point2{0.5, 0.5}, Point2{1, 0.5}, Point2{0.5, 1}};
        REQUIRE(t1.intersects(t_small));
        REQUIRE(t_small.intersects(t1));
    }
}

TEST_CASE("Triangle2 intersection - degenerate cases", "[Triangle2]") {
    const Triangle2 t{Point2{0, 0}, Point2{2, 0}, Point2{0, 2}};
    const Triangle2 t_segment{Point2{0, 0}, Point2{1, 1}, Point2{2, 2}};
    const Triangle2 t_point{Point2{1, 1}, Point2{1, 1}, Point2{1, 1}};
    const Triangle2 t_outside{Point2{3, 3}, Point2{4, 3}, Point2{3, 4}};

    SECTION("Triangle intersects segment") {
        REQUIRE(t.intersects(t_segment));
        REQUIRE(t_segment.intersects(t));
    }

    SECTION("Triangle intersects point inside") {
        REQUIRE(t.intersects(t_point));
        REQUIRE(t_point.intersects(t));
    }

    SECTION("Triangle and outside point do not intersect") {
        REQUIRE_FALSE(t_outside.intersects(t_point));
        REQUIRE_FALSE(t_point.intersects(t_outside));
    }
}

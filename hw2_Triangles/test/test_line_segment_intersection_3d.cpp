#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

import isshlyapin.triangle;
import isshlyapin.point;
import isshlyapin.line;
import isshlyapin.line_segment;

using Catch::Approx;
using namespace geometry;

TEST_CASE("Basic functionality LineSegment3", "[LineSegment3]") {
    const Point3 p1{0.0, 0.0, 0.0};
    const Point3 p2{1.0, 1.0, 1.0};

    const LineSegment3 ls1{p1, p2};
    REQUIRE_FALSE(ls1.is_degenerate());
    REQUIRE(ls1.get_type() == LineSegment3::Type::LineSegment);
    
    const Point3 tp1 = ls1.get_point(0);
    const Point3 tp2 = ls1.get_point(1);
    REQUIRE(tp1.x == Approx(0.0));
    REQUIRE(tp1.y == Approx(0.0));
    REQUIRE(tp1.z == Approx(0.0));
    REQUIRE(tp2.x == Approx(1.0));
    REQUIRE(tp2.y == Approx(1.0));
    REQUIRE(tp2.z == Approx(1.0));

    const LineSegment3 ls2{p1, p1};
    REQUIRE(ls2.is_degenerate());
    REQUIRE(ls2.get_type() == LineSegment3::Type::Point);

    REQUIRE(ls2.contains_point(p1));
    REQUIRE_FALSE(ls2.contains_point(Point3{-1, -1, -1}));

    REQUIRE(ls1.contains_point(Point3{0.5, 0.5, 0.5}));
    REQUIRE_FALSE(ls1.contains_point(Point3{-1, -1, -1}));

    REQUIRE(ls1.is_point_in_box(Point3{1, 0, 0}));
    REQUIRE(ls1.is_point_in_box(Point3{0, 1, 0}));
    REQUIRE(ls1.is_point_in_box(Point3{0, 0, 1}));

    REQUIRE(ls1.is_point_in_box(Point3{0.5, 0, 0}));
    REQUIRE(ls1.is_point_in_box(Point3{0, 0.5, 0}));
    REQUIRE(ls1.is_point_in_box(Point3{0, 0, 0.5}));

    REQUIRE_FALSE(ls1.is_point_in_box(Point3{2, 2, 2}));
}

TEST_CASE("Normal LineSegment3 intersection", "[LineSegment3]") {
    const LineSegment3 ls1{Point3{2, 0, 0}, Point3{0, 2, 0}};
    REQUIRE(ls1.intersects(ls1));

    const LineSegment3 ls2{Point3{-2, 0, 0}, Point3{0, -2, 0}};
    REQUIRE_FALSE(ls1.intersects(ls2));
    REQUIRE_FALSE(ls2.intersects(ls1));

    const LineSegment3 ls3{Point3{-2, 0, 2}, Point3{0, -2, 2}};
    REQUIRE_FALSE(ls1.intersects(ls3));
    REQUIRE_FALSE(ls3.intersects(ls1));

    const LineSegment3 ls4{Point3{3, 3, 2}, Point3{-3, -3, 2}};
    REQUIRE_FALSE(ls1.intersects(ls4));
    REQUIRE_FALSE(ls4.intersects(ls1));

    const LineSegment3 ls5{Point3{1, 1, 2}, Point3{1, 1, -2}};
    REQUIRE(ls1.intersects(ls5));
    REQUIRE(ls5.intersects(ls1));

    const LineSegment3 ls6{Point3{0, 0, 0}, Point3{3, 1, 0}};
    REQUIRE(ls1.intersects(ls6));
    REQUIRE(ls6.intersects(ls1));

    const LineSegment3 ls7{Point3{0, 0, 0}, Point3{1, 1, 0}};
    REQUIRE(ls1.intersects(ls7));
    REQUIRE(ls7.intersects(ls1));

    const LineSegment3 ls8{Point3{0, 0, 0}, Point3{2, 0, 0}};
    REQUIRE(ls1.intersects(ls8));
    REQUIRE(ls8.intersects(ls1));
}

TEST_CASE("Degenerate LineSegment3 intersection", "[LineSegment3]") {
    const LineSegment3 ls1{Point3{0, 0, 0}, Point3{0, 0, 0}};
    REQUIRE(ls1.intersects(ls1));
    
    const LineSegment3 ls2{Point3{2, 0, 0}, Point3{0, 2, 0}};
    REQUIRE_FALSE(ls1.intersects(ls2));
    REQUIRE_FALSE(ls2.intersects(ls1));

    const LineSegment3 ls3{Point3{0, 0, 0}, Point3{2, 2, 0}};
    REQUIRE(ls1.intersects(ls3));
    REQUIRE(ls3.intersects(ls1));

    const LineSegment3 ls4{Point3{-2, -2, -2}, Point3{2, 2, 2}};
    REQUIRE(ls1.intersects(ls4));
    REQUIRE(ls4.intersects(ls1));

    const LineSegment3 ls5{Point3{2, 2, 2}, Point3{2, 2, 2}};
    REQUIRE_FALSE(ls1.intersects(ls5));
    REQUIRE_FALSE(ls5.intersects(ls1));
}

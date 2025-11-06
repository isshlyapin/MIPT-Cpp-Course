#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

import isshlyapin.triangle;
import isshlyapin.point;
import isshlyapin.line_segment;

using Catch::Approx;
using namespace geometry;

TEST_CASE("Basic functionality LineSegment2", "[LineSegment2]") {
    const Point2 p1{0.0, 0.0};
    const Point2 p2{1.0, 1.0};

    const LineSegment2 ls1{p1, p2};
    REQUIRE_FALSE(ls1.is_degenerate());
    REQUIRE(ls1.get_type() == LineSegment2::Type::LineSegment);
    
    const Point2 tp1 = ls1.get_point(0);
    const Point2 tp2 = ls1.get_point(1);
    REQUIRE(tp1.x == Approx(0.0));
    REQUIRE(tp1.y == Approx(0.0));
    REQUIRE(tp2.x == Approx(1.0));
    REQUIRE(tp2.y == Approx(1.0));

    const LineSegment2 ls2{p1, p1};
    REQUIRE(ls2.is_degenerate());
    REQUIRE(ls2.get_type() == LineSegment2::Type::Point);
}

TEST_CASE("Normal LineSegment2 intersection", "[LineSegment2]") {
    const LineSegment2 ls1{Point2{0, 0}, Point2{1, 1}};
    REQUIRE(ls1.intersects(ls1));

    const LineSegment2 ls2{Point2{1, 0}, Point2{1, -1}};
    REQUIRE_FALSE(ls1.intersects(ls2));
    REQUIRE_FALSE(ls2.intersects(ls1));

    const LineSegment2 ls3{Point2{1, 0}, Point2{0, 1}};
    REQUIRE(ls1.intersects(ls3));
    REQUIRE(ls3.intersects(ls1));

    const LineSegment2 ls4{Point2{0, 0}, Point2{-1, -1}};
    REQUIRE(ls1.intersects(ls4));
    REQUIRE(ls4.intersects(ls1));

    const LineSegment2 ls5{Point2{0.5, 0.5}, Point2{1, 0}};
    REQUIRE(ls1.intersects(ls5));
    REQUIRE(ls5.intersects(ls1));
    
    const LineSegment2 ls6{Point2{0, 0}, Point2{2, 2}};
    REQUIRE(ls1.intersects(ls6));
    REQUIRE(ls5.intersects(ls6));
}

TEST_CASE("Degenerate LineSegment2 intersection", "[LineSegment2]") {
    const LineSegment2 ls1{Point2{0, 0}, Point2{0, 0}};
    REQUIRE(ls1.intersects(ls1));

    const LineSegment2 ls2{Point2{1, 0}, Point2{0, 1}};
    REQUIRE_FALSE(ls1.intersects(ls2));
    REQUIRE_FALSE(ls2.intersects(ls1));

    const LineSegment2 ls3{Point2{0, 0}, Point2{1, 1}};
    REQUIRE(ls1.intersects(ls3));
    REQUIRE(ls3.intersects(ls1));

    const LineSegment2 ls4{Point2{-1, -1}, Point2{1, 1}};
    REQUIRE(ls1.intersects(ls4));
    REQUIRE(ls4.intersects(ls1));

    const LineSegment2 ls5{Point2{1, 1}, Point2{1, 1}};
    REQUIRE_FALSE(ls1.intersects(ls5));
    REQUIRE_FALSE(ls5.intersects(ls1));
}

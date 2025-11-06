#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

import isshlyapin.triangle;
import isshlyapin.point;
import isshlyapin.vector;
import isshlyapin.line;

using Catch::Approx;
using namespace geometry;

TEST_CASE("Basic functionality Line", "[Line]") {
    REQUIRE_THROWS(Line3{Vector3{0, 0, 0}, Point3{1, 1, 1}});

    const Line3 l1{Vector3{1, 0, 0}, Point3{0, 0, 1}};
    const Line3 l2{Vector3{0, 1, 0}, Point3{0, 0, -1}};
    REQUIRE_FALSE(l1.intersects(l2));
    REQUIRE_FALSE(l2.intersects(l1));

    const Line3 l3{Vector3{1, 0, 0}, Point3{0, 1, 1}};
    REQUIRE_FALSE(l1.intersects(l3));
    REQUIRE_FALSE(l3.intersects(l1));

    const Line3 l4{Vector3{1, 0, 0}, Point3{0, 0, 1}};
    REQUIRE(l1.intersects(l4));
    REQUIRE(l4.intersects(l4));
    
    auto res = l1.intersects(l4);
    REQUIRE(res.value().x == Approx(0));
    REQUIRE(res.value().y == Approx(0));
    REQUIRE(res.value().z == Approx(1));

    const Line3 l5{Vector3{1, 1, 1}, Point3{1, 1, 1}};
    const Line3 l6{Vector3{-1, -1, 2}, Point3{-1, -1, 2}};
    REQUIRE(l5.intersects(l6));
    REQUIRE(l6.intersects(l5));

    res = l5.intersects(l6);
    REQUIRE(res.value().x == Approx(0));
    REQUIRE(res.value().y == Approx(0));
    REQUIRE(res.value().z == Approx(0));
}

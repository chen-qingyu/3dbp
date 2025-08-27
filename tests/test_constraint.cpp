#include <catch2/catch_test_macros.hpp>

#include "../sources/constraint.hpp"

TEST_CASE("Constraint")
{
    Container container{"container", 10, 10, 10, 100.0};
    std::vector<Box> boxes = {{"origin", 5, 5, 5, 0, 0, 0, 10.0}};
    Box box_x{"bx", 5, 5, 5, 5, 0, 0, 10.0};
    Box box_y{"by", 5, 5, 5, 0, 5, 0, 10.0};
    Box box_z{"bz", 5, 5, 5, 0, 0, 5, 10.0};

    SECTION("check_bound")
    {
        Constraint constraint(container, boxes);

        Box box_bad{"bad", 5, 5, 6, 0, 0, 5, 10.0}; // Exceeds container height
        REQUIRE(constraint.check_bound(box_x));
        REQUIRE(constraint.check_bound(box_y));
        REQUIRE(constraint.check_bound(box_z));
        REQUIRE_FALSE(constraint.check_bound(box_bad));
    }

    SECTION("check_overlap")
    {
        Constraint constraint(container, boxes);

        Box box_bad{"bad", 5, 5, 5, 0, 0, 4, 10.0}; // Overlaps with origin box
        REQUIRE(constraint.check_overlap(box_x));
        REQUIRE(constraint.check_overlap(box_y));
        REQUIRE(constraint.check_overlap(box_z));
        REQUIRE_FALSE(constraint.check_overlap(box_bad));
    }

    SECTION("check_support")
    {
        Constraint constraint(container, boxes);

        Box box_bad{"bad", 5, 5, 5, 5, 0, 1, 10.0}; // Not properly supported
        REQUIRE(constraint.check_support(box_x));
        REQUIRE(constraint.check_support(box_y));
        REQUIRE(constraint.check_support(box_z));
        REQUIRE_FALSE(constraint.check_support(box_bad));
    }

    SECTION("check_weight")
    {
        Constraint constraint(container, boxes);

        Box box_bad{"bad", 5, 5, 5, 5, 0, 0, 100.0}; // Exceeds weight limit
        REQUIRE(constraint.check_weight(box_x));
        REQUIRE(constraint.check_weight(box_y));
        REQUIRE(constraint.check_weight(box_z));
        REQUIRE_FALSE(constraint.check_weight(box_bad));
    }
}

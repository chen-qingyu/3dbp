#include <catch2/catch_test_macros.hpp>

#include "../sources/constraint.hpp"

TEST_CASE("Constraint")
{
    Container container{"container", 10, 10, 10, 100.0};
    auto type = std::make_shared<BoxType>("t1", 5, 5, 5);
    std::vector<Box> boxes = {{"origin", type, 10.0, "", 0, 0, 0}};
    Box box_x{"bx", type, 10.0, "", 5, 0, 0};
    Box box_y{"by", type, 10.0, "", 0, 5, 0};
    Box box_z{"bz", type, 10.0, "", 0, 0, 5};

    SECTION("check_bound")
    {
        Constraint constraint(container, boxes);

        Box box_bad{"bad", type, 10.0, "", 0, 0, 6}; // Exceeds container height
        REQUIRE(constraint.check_bound(box_x));
        REQUIRE(constraint.check_bound(box_y));
        REQUIRE(constraint.check_bound(box_z));
        REQUIRE_FALSE(constraint.check_bound(box_bad));
    }

    SECTION("check_overlap")
    {
        Constraint constraint(container, boxes);

        Box box_bad{"bad", type, 10.0, "", 0, 0, 4}; // Overlaps with origin box
        REQUIRE(constraint.check_overlap(box_x));
        REQUIRE(constraint.check_overlap(box_y));
        REQUIRE(constraint.check_overlap(box_z));
        REQUIRE_FALSE(constraint.check_overlap(box_bad));
    }

    SECTION("check_support")
    {
        Constraint constraint(container, boxes);

        Box box_bad{"bad", type, 10.0, "", 5, 0, 1}; // Not properly supported
        REQUIRE(constraint.check_support(box_x));
        REQUIRE(constraint.check_support(box_y));
        REQUIRE(constraint.check_support(box_z));
        REQUIRE_FALSE(constraint.check_support(box_bad));
    }

    SECTION("check_weight")
    {
        Constraint constraint(container, boxes);

        Box box_bad{"bad", type, 100.0, "", 5, 0, 0}; // Exceeds weight limit
        REQUIRE(constraint.check_weight(box_x));
        REQUIRE(constraint.check_weight(box_y));
        REQUIRE(constraint.check_weight(box_z));
        REQUIRE_FALSE(constraint.check_weight(box_bad));
    }
}

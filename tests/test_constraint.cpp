#include <catch2/catch_test_macros.hpp>

#include "../sources/constraint.hpp"

Box make_box(std::string id, std::shared_ptr<BoxType> type, double weight, int x, int y, int z)
{
    return Box{id, type->id, weight, x, y, z, Orient::UNDEFINED, type, type->lx, type->ly, type->lz};
}

TEST_CASE("Constraint")
{
    Container container(ContainerType{"ct1", 10, 10, 10, 100.0, 1});
    auto type = std::make_shared<BoxType>("t1", 5, 5, 5);
    std::vector<Box> boxes = {make_box("origin", type, 10.0, 0, 0, 0)};
    Box box_x = make_box("bx", type, 10.0, 5, 0, 0);
    Box box_y = make_box("by", type, 10.0, 0, 5, 0);
    Box box_z = make_box("bz", type, 10.0, 0, 0, 5);

    SECTION("check_bound")
    {
        Constraint constraint(container, boxes);

        Box box_bad = make_box("bad", type, 10.0, 0, 0, 6); // Exceeds container height
        REQUIRE(constraint.check_bound(box_x));
        REQUIRE(constraint.check_bound(box_y));
        REQUIRE(constraint.check_bound(box_z));
        REQUIRE_FALSE(constraint.check_bound(box_bad));
    }

    SECTION("check_overlap")
    {
        Constraint constraint(container, boxes);

        Box box_bad = make_box("bad", type, 10.0, 0, 0, 4); // Overlaps with origin box
        REQUIRE(constraint.check_overlap(box_x));
        REQUIRE(constraint.check_overlap(box_y));
        REQUIRE(constraint.check_overlap(box_z));
        REQUIRE_FALSE(constraint.check_overlap(box_bad));
    }

    SECTION("check_support")
    {
        Constraint constraint(container, boxes);

        Box box_bad = make_box("bad", type, 10.0, 5, 0, 1); // Not properly supported
        REQUIRE(constraint.check_support(box_x));
        REQUIRE(constraint.check_support(box_y));
        REQUIRE(constraint.check_support(box_z));
        REQUIRE_FALSE(constraint.check_support(box_bad));
    }

    SECTION("check_weight")
    {
        Constraint constraint(container, boxes);

        Box box_bad = make_box("bad", type, 100.0, 5, 0, 0); // Exceeds weight limit
        REQUIRE(constraint.check_weight(box_x));
        REQUIRE(constraint.check_weight(box_y));
        REQUIRE(constraint.check_weight(box_z));
        REQUIRE_FALSE(constraint.check_weight(box_bad));
    }
}

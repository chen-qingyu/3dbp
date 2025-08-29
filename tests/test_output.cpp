#include <filesystem>
#include <fstream>
#include <unordered_set>

#include <catch2/catch_test_macros.hpp>
#include <nlohmann/json-schema.hpp>

#include "../sources/algorithm.hpp"

TEST_CASE("Output")
{
    std::filesystem::create_directories("result");

    for (const auto& input_file : std::filesystem::directory_iterator("data/tests"))
    {
        // 运行算法并输出结果到文件
        json input = json::parse(std::ifstream(input_file.path()));
        Output data = Simple(input).run();
        auto output_file = "result/result-" + input_file.path().filename().string();
        std::ofstream(output_file) << json(data).dump(2);

        // 1. 校验输出数据JSON格式
        json output = json::parse(std::ifstream(output_file));
        json schema = json::parse(std::ifstream("tests/output_schema.json"));
        nlohmann::json_schema::json_validator validator(schema);
        REQUIRE_NOTHROW(validator.validate(output));

        // 2. 确保输入箱子与输出箱子数量相等
        int out_box_cnt = std::accumulate(output["plans"].begin(), output["plans"].end(),
                                          int(output["unpacked_boxes"].size()),
                                          [](int sum, const json& plan)
                                          { return sum + int(plan["boxes"].size()); });
        REQUIRE(int(input["boxes"].size()) == out_box_cnt);

        // 3. 确保输入箱子与输出箱子实体相等
        std::unordered_set<Box> input_boxes = input["boxes"];
        std::unordered_set<Box> output_boxes = output["unpacked_boxes"];
        for (const auto& plan : output["plans"])
        {
            output_boxes.insert(plan["boxes"].begin(), plan["boxes"].end());
        }
        REQUIRE(input_boxes == output_boxes);

        // 4. 确保输出的每个已装载箱子都符合约束
        for (const auto& plan : output["plans"])
        {
            Container container = plan["container"];
            for (const auto& box : plan["boxes"])
            {
                std::vector<Box> boxes = plan["boxes"];
                boxes.erase(std::find(boxes.begin(), boxes.end(), box));
                Constraint constraint(container, boxes);
                REQUIRE(constraint.check_constraints(box));
            }
        }
    }
}

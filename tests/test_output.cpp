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
        // 运行算法并输出结果
        spdlog::set_level(spdlog::level::off);
        json data = json::parse(std::ifstream(input_file.path()));
        Output output = Algorithm(data).run();

        // 1. 确保输入箱子与输出箱子数量相等
        int out_box_cnt = std::accumulate(output.containers.begin(), output.containers.end(),
                                          int(output.unpacked_boxes.size()),
                                          [](int sum, const Container& container)
                                          { return sum + int(container.boxes.size()); });
        REQUIRE(int(data["boxes"].size()) == out_box_cnt);

        // 2. 确保输入箱子与输出箱子实体相等
        std::unordered_set<Box> input_boxes = data["boxes"];
        std::unordered_set<Box> output_boxes(output.unpacked_boxes.begin(), output.unpacked_boxes.end());
        for (const auto& container : output.containers)
        {
            output_boxes.insert(container.boxes.begin(), container.boxes.end());
        }
        REQUIRE(input_boxes == output_boxes);

        // 3. 确保输出的每个已装载箱子都符合约束
        for (const auto& container : output.containers)
        {
            for (const auto& box : container.boxes)
            {
                std::vector<Box> boxes = container.boxes;
                boxes.erase(std::find(boxes.begin(), boxes.end(), box));
                Constraint constraint(container, boxes);
                REQUIRE(constraint.check_constraints(box));
            }
        }

        // 4. 校验输出数据JSON格式
        auto output_file = "result/result-" + input_file.path().filename().string();
        std::ofstream(output_file) << json(output).dump(2);
        json result = json::parse(std::ifstream(output_file));
        json schema = json::parse(std::ifstream("tests/output_schema.json"));
        nlohmann::json_schema::json_validator validator(schema);
        REQUIRE_NOTHROW(validator.validate(result));
    }
}

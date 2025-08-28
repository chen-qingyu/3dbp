#include <filesystem>
#include <fstream>

#include <catch2/catch_test_macros.hpp>
#include <nlohmann/json-schema.hpp>

#include "../sources/algorithm.hpp"

TEST_CASE("Schema")
{
    // 输出结果到文件
    using std::filesystem::path;
    Output data = Simple(json::parse(std::ifstream("data/demo.json"))).run();
    path file = path("result") / "result-test.json";
    std::filesystem::create_directories(file.parent_path());
    std::ofstream(file) << json(data).dump(2);

    // 校验JSON输出数据格式
    json output = json::parse(std::ifstream(file));
    json schema = json::parse(std::ifstream("tests/output_schema.json"));
    nlohmann::json_schema::json_validator validator(schema);

    REQUIRE_NOTHROW(validator.validate(output));
}

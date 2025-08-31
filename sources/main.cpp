#include <filesystem>
#include <fstream>
#include <string>

#include <spdlog/spdlog.h>

#include "algorithm.hpp"
#include "tool.hpp"

using std::filesystem::path;

int main(int argc, char** argv)
{
    // 检查参数
    if (argc != 2)
    {
        spdlog::error("Usage: {} <input_json>", path(argv[0]).filename().string());
        return 1;
    }

    // 读取输入
    std::string input = argv[1];
    std::ifstream input_file(input);
    if (!input_file.is_open())
    {
        spdlog::error("Failed to open input file \"{}\".", input);
        return 1;
    }
    json input_json = json::parse(input_file);
    input_file.close();

    // 校验输入
    validate_schema(input_json);
    Input input_obj = input_json;
    validate_logic(input_obj);
    spdlog::info("Successfully validated input \"{}\".", input);

    // 算法处理
    Output output_data = Algorithm(input_obj).run();

    // 输出结果
    path output = path("result") / ("result-" + path(input).filename().string());
    std::filesystem::create_directories(output.parent_path());
    std::ofstream(output) << json(output_data).dump(2);
    spdlog::info("Results written to \"{}\".", output.string());

    return 0;
}

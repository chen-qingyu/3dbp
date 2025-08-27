#include <filesystem>
#include <fstream>
#include <string>

#include <spdlog/spdlog.h>

#include "algorithm.hpp"
#include "tool.hpp"

using std::filesystem::path;

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        spdlog::error("Usage: {} <input_json>", path(argv[0]).filename().string());
        return 1;
    }

    // 读取输入
    std::string file = argv[1];
    std::ifstream input(file);
    if (!input.is_open())
    {
        spdlog::error("Failed to open input file \"{}\".", file);
        return 1;
    }
    json input_json = json::parse(input);
    input.close();
    validate_schema(input_json);
    Input input_data = input_json;
    validate_logic(input_data);
    spdlog::info("Successfully validated input \"{}\".", file);

    // 算法处理
    Output output_data = Simple(input_data).run();

    // 输出结果
    path output = path("result") / ("result-" + path(file).filename().string());
    std::filesystem::create_directories(output.parent_path());
    std::ofstream(output) << json(output_data).dump(2);
    spdlog::info("Results written to \"{}\".", output.string());

    return 0;
}

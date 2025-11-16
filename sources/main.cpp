#include <filesystem>
#include <fstream>
#include <string>

#include <spdlog/spdlog.h>

#include "algorithm.hpp"
#include "tool.hpp"

using std::filesystem::path;

int main(int argc, char** argv)
{
    // 解析输入
    if (argc != 2)
    {
        spdlog::error("Usage: {} <input_json>", path(argv[0]).filename().string());
        return 1;
    }
    std::string input = argv[1];

    // 算法处理
    Output output_data = Algorithm(parse_input(input)).run();

    // 输出结果
    path output = path("result") / ("result-" + path(input).filename().string());
    std::filesystem::create_directories(output.parent_path());
    std::ofstream(output) << json(output_data).dump(2);
    spdlog::info("Results written to \"{}\".", output.string());

    return 0;
}

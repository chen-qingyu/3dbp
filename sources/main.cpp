#include <filesystem>
#include <fstream>
#include <string>

#include <spdlog/spdlog.h>

#include "entities.hpp"

int main(int argc, char** argv)
{
    std::string file = "demo.json";
    std::string input_dir = "data/";
    std::string output_dir = "result/";

    // 读取输入
    std::ifstream input(input_dir + file);
    if (!input.is_open())
    {
        spdlog::error("Failed to open input file \"{}\".", file);
        return 1;
    }
    Input input_data = json::parse(input);
    input.close();
    spdlog::info("Successfully parsed input file \"{}\".", file);

    // 元启发式算法处理
    Output output_data;

    // 输出结果到文件
    std::filesystem::create_directories(output_dir);
    std::ofstream output(output_dir + "result-" + file);
    output << json(output_data).dump(2);
    output.close();
    spdlog::info("Results written to \"{}\".", output_dir + "result-" + file);

    return 0;
}

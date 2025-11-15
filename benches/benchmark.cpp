#include <filesystem>
#include <fstream>

#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

#include "../sources/algorithm.hpp"

TEST_CASE("Benchmark", "[bench]")
{
    spdlog::set_level(spdlog::level::off);
    for (const auto& entry : std::filesystem::directory_iterator("data/br_json"))
    {
        if (entry.path().string().find("001") == std::string::npos)
        {
            continue;
        }
        BENCHMARK(entry.path().filename().string())
        {
            Input input = json::parse(std::ifstream(entry.path()));
            Output output = Algorithm(input).run();
            return output.containers.size(); // 返回方案数量，防止优化
        };
    }
}

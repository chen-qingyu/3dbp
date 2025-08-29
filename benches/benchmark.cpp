#include <filesystem>
#include <fstream>

#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

#include "../sources/algorithm.hpp"

TEST_CASE("Benchmark")
{
    spdlog::set_level(spdlog::level::off);
    int count = 0;
    for (const auto& entry : std::filesystem::directory_iterator("data/br_json"))
    {
        if (++count > 10)
        {
            break;
        }
        BENCHMARK(entry.path().string())
        {
            Input input = json::parse(std::ifstream(entry.path()));
            Output output = Simple(input).run();
            return output.plans.size(); // 返回方案数量，防止优化
        };
    }
}

// xmake config -m release && xmake run bench --benchmark-no-analysis

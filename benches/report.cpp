#include <filesystem>
#include <fstream>

#include <catch2/catch_test_macros.hpp>

#include "../sources/algorithm.hpp"

TEST_CASE("Report", "[report]")
{
    spdlog::set_level(spdlog::level::off);

    std::vector<std::pair<std::string, double>> results;
    for (const auto& entry : std::filesystem::directory_iterator("data/br_json"))
    {
        json input = json::parse(std::ifstream(entry.path()));
        Output data = Algorithm(input).run();
        double volume_rate = data.containers[0].volume_rate;
        results.emplace_back(entry.path().filename().string(), volume_rate);
    }

    std::ofstream csv("benches/report.csv");
    csv << "file_name,volume_rate\n";
    for (const auto& [name, rate] : results)
    {
        csv << name << ',' << rate << '\n';
    }
}

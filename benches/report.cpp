#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <numeric>

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

    // Write CSV
    std::ofstream csv("benches/report.csv");
    csv << "file_name,volume_rate\n";
    for (const auto& [name, rate] : results)
    {
        csv << name << ',' << rate << '\n';
    }

    // Calculate statistics
    double sum = std::accumulate(results.begin(), results.end(), 0.0,
                                 [](double sum, const auto& result)
                                 { return sum + result.second; });
    double average = sum / results.size();

    auto [min_it, max_it] = std::minmax_element(results.begin(), results.end(),
                                                [](const auto& a, const auto& b)
                                                { return a.second < b.second; });

    std::ofstream report("benches/report.txt");
    report << std::fixed << std::setprecision(6);
    report << "Statistics Summary\n";
    report << "===\n";
    report << "Total files processed: " << results.size() << "\n";
    report << "Average volume rate: " << average << "\n";
    report << "Minimum volume rate: " << min_it->second << " (file: " << min_it->first << ")\n";
    report << "Maximum volume rate: " << max_it->second << " (file: " << max_it->first << ")\n";
}

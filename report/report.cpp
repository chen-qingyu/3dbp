#include <algorithm>
#include <filesystem>
#include <format>
#include <fstream>
#include <map>
#include <numeric>

#include "../sources/algorithm.hpp"

int main()
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
    std::ofstream csv("report/report.csv");
    csv << "file_name,volume_rate\n";
    for (const auto& [name, rate] : results)
    {
        csv << name << ',' << rate << '\n';
    }

    // Calculate statistics
    std::map<std::string, std::vector<double>> br_file;
    for (const auto& [name, rate] : results)
    {
        br_file[name.substr(0, 4)].push_back(rate);
    }
    double sum = std::accumulate(results.begin(), results.end(), 0.0, [](double sum, const auto& result)
                                 { return sum + result.second; });
    double average = sum / results.size();

    auto [min_it, max_it] = std::minmax_element(results.begin(), results.end(),
                                                [](const auto& a, const auto& b)
                                                { return a.second < b.second; });

    std::ofstream report("report/report.txt");
    report << "Statistics Summary\n";
    report << "===\n";
    for (const auto& [file, rates] : br_file)
    {
        double file_avg = std::accumulate(rates.begin(), rates.end(), 0.0) / rates.size();
        report << std::format("{}: {:.6f} ({} cases)\n", file, file_avg, rates.size());
    }
    report << "===\n";
    report << std::format("Total files processed: {}\n", results.size());
    report << std::format("Average volume rate: {:.6f}\n", average);
    report << std::format("Minimum volume rate: {:.6f} (file: {})\n", min_it->second, min_it->first);
    report << std::format("Maximum volume rate: {:.6f} (file: {})\n", max_it->second, max_it->first);

    return 0;
}

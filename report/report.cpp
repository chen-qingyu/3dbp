#include <algorithm>
#include <chrono>
#include <filesystem>
#include <format>
#include <fstream>
#include <map>
#include <numeric>
#include <thread>

#include <spdlog/spdlog.h>

#include "../sources/algorithm.hpp"

#ifdef _WIN32
#include <psapi.h>
#include <windows.h>
#endif

// 文件测试结果
struct FileResult
{
    std::string filename; // 文件名
    double rate;          // 体积利用率
    double duration;      // 耗时(秒)
    double memory;        // 内存占用(KB)
};

// 统计数据
struct Statistics
{
    double mean;
    double min;
    double max;
};

// 获取当前进程内存使用量(KB)
double get_mem_usage()
{
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS_EX pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc)))
    {
        return pmc.WorkingSetSize / 1024.0;
    }
#endif
    return 0;
}

// 清理内存
void clean_memory()
{
#ifdef _WIN32
    EmptyWorkingSet(GetCurrentProcess());
    std::this_thread::sleep_for(std::chrono::milliseconds(10)); // 短暂休眠，让系统有时间处理
#endif
}

// 计算统计信息
Statistics calc_stats(const std::vector<double>& data)
{
    double sum = std::accumulate(data.begin(), data.end(), 0.0);
    double mean = sum / data.size();
    double min = *std::min_element(data.begin(), data.end());
    double max = *std::max_element(data.begin(), data.end());
    return {mean, min, max};
}

int main()
{
    spdlog::set_level(spdlog::level::off);
    std::filesystem::path dir("data/br_json/");

    // 单线程顺序处理每个文件，避免多线程导致内存统计不准
    std::vector<FileResult> report;
    for (const auto& entry : std::filesystem::directory_iterator(dir))
    {
        if (!entry.is_regular_file())
        {
            continue;
        }

        // 清理内存，记录起始时间和内存
        clean_memory();
        auto start_time = std::chrono::high_resolution_clock::now();
        auto mem_before = get_mem_usage();

        // 读取输入文件
        json input = json::parse(std::ifstream(entry.path()));

        // 执行装箱算法
        Output data = Algorithm(input).run();

        // 记录各项指标
        auto end_time = std::chrono::high_resolution_clock::now();
        auto mem_after = get_mem_usage();
        auto filename = entry.path().filename().string();
        auto rate = data.containers[0].volume_rate;
        auto duration = std::chrono::duration<double>(end_time - start_time).count();
        auto memory = mem_after - mem_before;

        report.push_back({filename, rate, duration, memory});

        spdlog::set_level(spdlog::level::info);
        spdlog::info("{} - rate: {:.2f}%, duration: {:.3f} s, memory: {:.0f} KB", filename, rate * 100, duration,
                     memory);
        spdlog::set_level(spdlog::level::off);
    }

    // 收集各项数据
    std::vector<double> rates;
    std::vector<double> durations;
    std::vector<double> memories;
    for (const auto& result : report)
    {
        rates.push_back(result.rate);
        durations.push_back(result.duration);
        memories.push_back(result.memory);
    }

    // 写入CSV
    std::ofstream csv("report/report.csv");
    csv << "filename,volume_rate(%),duration(s),memory(KB)\n";
    for (const auto& result : report)
    {
        csv << result.filename << ',' << std::format("{:.2f}", result.rate * 100) << ','
            << std::format("{:.3f}", result.duration) << ',' << std::format("{:.0f}", result.memory) << '\n';
    }
    csv.close();

    // 计算统计信息
    auto rat_stats = calc_stats(rates);
    auto dur_stats = calc_stats(durations);
    auto mem_stats = calc_stats(memories);

    // 按文件前缀分组统计（br00_001.json -> br00）
    std::map<std::string, std::vector<double>> group_rate;
    std::map<std::string, std::vector<double>> group_duration;
    std::map<std::string, std::vector<double>> group_memory;
    for (const auto& result : report)
    {
        std::string prefix = result.filename.substr(0, 4);
        group_rate[prefix].push_back(result.rate);
        group_duration[prefix].push_back(result.duration);
        group_memory[prefix].push_back(result.memory);
    }

    // 生成统计报告文本
    std::string text = "Statistics Summary\n";
    text += std::format("Total Count: {}\n", report.size());
    text += std::format("Volume Rate: mean {:.2f}%, min {:.2f}%, max {:.2f}%\n", rat_stats.mean * 100,
                        rat_stats.min * 100, rat_stats.max * 100);
    text +=
        std::format("Duration: mean {:.3f} s, min {:.3f} s, max {:.3f} s\n", dur_stats.mean, dur_stats.min, dur_stats.max);
    text += std::format("Memory: mean {:.0f} KB, min {:.0f} KB, max {:.0f} KB\n", mem_stats.mean, mem_stats.min,
                        mem_stats.max);
    text += "\nGrouped by file:\n";
    for (const auto& [prefix, rate] : group_rate)
    {
        auto group_rat_stats = calc_stats(rate);
        auto group_dur_stats = calc_stats(group_duration[prefix]);
        auto group_mem_stats = calc_stats(group_memory[prefix]);

        text += std::format("File {} Count: {}\n", prefix, rate.size());
        text += std::format("  Volume Rate: mean {:.2f}%, min {:.2f}%, max {:.2f}%\n", group_rat_stats.mean * 100,
                            group_rat_stats.min * 100, group_rat_stats.max * 100);
        text += std::format("  Duration: mean {:.3f} s, min {:.3f} s, max {:.3f} s\n", group_dur_stats.mean,
                            group_dur_stats.min, group_dur_stats.max);
        text += std::format("  Memory: mean {:.0f} KB, min {:.0f} KB, max {:.0f} KB\n", group_mem_stats.mean,
                            group_mem_stats.min, group_mem_stats.max);
    }

    // 写入文件
    std::ofstream txt("report/report.txt");
    txt << text;
    txt.close();

    return 0;
}

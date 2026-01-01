#include <chrono>
#include <filesystem>
#include <format>
#include <fstream>
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
        auto rate = data.containers[0].volume_rate * 100;
        auto duration = std::chrono::duration<double>(end_time - start_time).count();
        auto memory = mem_after - mem_before;

        report.push_back({filename, rate, duration, memory});

        spdlog::set_level(spdlog::level::info);
        spdlog::info("{} - rate: {:.2f}%, duration: {:.3f} s, memory: {:.0f} KB", filename, rate, duration,
                     memory);
        spdlog::set_level(spdlog::level::off);
    }

    // 写入CSV
    std::ofstream csv("report/report.csv");
    csv << "filename,volume_rate(%),duration(s),memory(KB)\n";
    for (const auto& result : report)
    {
        csv << result.filename << ',' << std::format("{:.2f}", result.rate) << ','
            << std::format("{:.3f}", result.duration) << ',' << std::format("{:.0f}", result.memory) << '\n';
    }
    csv.close();

    return 0;
}

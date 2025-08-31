#ifndef ALGORITHM_HPP
#define ALGORITHM_HPP

#include <algorithm>
#include <ranges>
#include <tuple>
#include <vector>

#include <spdlog/spdlog.h>

#include "constraint.hpp"

/// 装箱算法入口
class Algorithm
{
private:
    std::vector<Container> containers_; // 可用容器
    Container container_;               // 当前容器
    std::vector<Box> unpacked_boxes_;   // 待装载箱子
    std::vector<Box> packed_boxes_;     // 已装载箱子
    Constraint constraint_;             // 约束检查

    /// 尝试装载箱子
    /// @param box 待装载的箱子（会被修改位置）
    /// @return 是否成功装载
    bool try_pack(Box& box) const
    {
        // 包括原点（左前角）在内的所有可能的装载位置（候选点）
        std::vector<std::tuple<int, int, int>> candidates{{0, 0, 0}};

        // 在已装载箱子的右方、后方、上方生成候选点
        for (const auto& box : packed_boxes_)
        {
            candidates.emplace_back(box.x + box.lx, box.y, box.z); // 右方
            candidates.emplace_back(box.x, box.y + box.ly, box.z); // 后方
            candidates.emplace_back(box.x, box.y, box.z + box.lz); // 上方
        }

        // 排序候选位置，优先级：z（高度/上方）> y（宽度/后方）> x（长度/右方）
        std::ranges::sort(candidates);

        // 尝试每个候选位置
        for (const auto& candidate : candidates)
        {
            box.x = std::get<0>(candidate);
            box.y = std::get<1>(candidate);
            box.z = std::get<2>(candidate);

            // 检查所有约束
            if (constraint_.check_constraints(box))
            {
                return true; // 找到有效位置
            }
        }

        return false; // 没有找到有效位置
    }

    /// 计算体积利用率
    /// @return 体积利用率（0-1之间）
    double calculate_volume_rate() const
    {
        long long used_volume = 0;
        for (const auto& box : packed_boxes_)
        {
            used_volume += box.volume();
        }
        return static_cast<double>(used_volume) / container_.volume();
    }

    /// 计算重量利用率
    /// @return 重量利用率（0-1之间）
    double calculate_weight_rate() const
    {
        if (isnan(container_.payload))
        {
            return NAN; // 容器未设置载重
        }

        double used_weight = 0;
        for (const auto& box : packed_boxes_)
        {
            used_weight += box.weight;
        }
        return used_weight / container_.payload;
    }

    /// 选择合适的容器（目的是体积利用率尽可能高）
    /// @return 选择的容器
    Container select_container() const
    {
        // 计算剩余箱子总体积
        long long unpacked_volume = 0;
        for (const auto& box : unpacked_boxes_)
        {
            unpacked_volume += box.volume();
        }
        // 寻找合适的容器
        for (const auto& container : containers_)
        {
            if (container.volume() >= unpacked_volume)
            {
                return container;
            }
        }
        return containers_.back(); // 如果没有合适的容器，使用最大的一个
    }

public:
    /// 构造函数
    /// @param input 输入数据
    Algorithm(const Input& input)
        : containers_(input.containers)
        , unpacked_boxes_(input.boxes)
        , packed_boxes_()
        , container_()
        , constraint_(container_, packed_boxes_)
    {
        // 箱子按体积从大到小排序
        std::ranges::sort(unpacked_boxes_, [](const auto& a, const auto& b)
                          { return a.volume() > b.volume(); });

        // 容器按体积从小到大排序
        std::ranges::sort(containers_, [](const auto& a, const auto& b)
                          { return a.volume() < b.volume(); });
    }

    /// 执行装箱算法
    /// @return 装箱结果
    Output run()
    {
        Output output;

        // 逐个容器装箱，直到容器用完或箱子装完
        while (!containers_.empty() && !unpacked_boxes_.empty())
        {
            // 选择合适的容器
            container_ = select_container();
            // 清空已装载的箱子
            packed_boxes_.clear();

            // 尝试装载箱子
            for (auto& box : unpacked_boxes_)
            {
                if (try_pack(box))
                {
                    packed_boxes_.push_back(box);
                }
            }

            spdlog::info("Packed {} boxes in container \"{}\".", packed_boxes_.size(), container_.id);

            // 更新剩余箱子列表
            std::erase_if(unpacked_boxes_, [&](const Box& box)
                          { return std::ranges::find(packed_boxes_, box) != packed_boxes_.end(); });
            // 更新可用容器列表
            containers_.erase(std::find(containers_.begin(), containers_.end(), container_));

            // 创建装箱方案
            Plan plan;
            plan.container = container_;
            plan.boxes = packed_boxes_;
            plan.volume_rate = calculate_volume_rate();
            plan.weight_rate = calculate_weight_rate();
            output.plans.push_back(plan);
        }
        // 重置未装载箱子的位置
        for (auto& box : unpacked_boxes_)
        {
            box.x = -1;
            box.y = -1;
            box.z = -1;
        }
        output.unpacked_boxes = unpacked_boxes_;
        spdlog::info("Unpacked {} boxes.", output.unpacked_boxes.size());

        return output;
    }
};

#endif // ALGORITHM_HPP

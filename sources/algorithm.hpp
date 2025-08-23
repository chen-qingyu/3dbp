#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <algorithm>
#include <tuple>
#include <vector>

#include <spdlog/spdlog.h>

#include "constraint.hpp"

/// 简单装箱算法
class Simple
{
private:
    std::vector<Container> containers_; // 可用载具
    Container container_;               // 当前载具
    std::vector<Box> remaining_boxes_;  // 待放置箱子
    std::vector<Box> placed_boxes_;     // 已放置箱子
    Constraint constraint_;             // 约束检查

    /// 尝试放置箱子
    /// @param box 待放置的箱子（会被修改位置）
    /// @return 是否成功放置
    bool try_place(Box& box) const
    {
        // 生成所有可能的放置位置（候选点）
        std::vector<std::tuple<int, int, int>> candidates;

        // 添加原点（左后角）作为候选位置
        candidates.emplace_back(0, 0, 0);

        // 基于已放置箱子生成候选位置
        for (const auto& box : placed_boxes_)
        {
            // 在已放置箱子的右方、前方、上方生成候选点
            candidates.emplace_back(box.x + box.lx, box.y, box.z); // 右方
            candidates.emplace_back(box.x, box.y + box.ly, box.z); // 前方
            candidates.emplace_back(box.x, box.y, box.z + box.lz); // 上方
        }

        // 排序候选位置，优先级：z（高度/上方）> y（宽度/前方）> x（长度/右方）
        std::sort(candidates.begin(), candidates.end());

        // 尝试每个候选位置
        for (const auto& candidate : candidates)
        {
            box.x = std::get<0>(candidate);
            box.y = std::get<1>(candidate);
            box.z = std::get<2>(candidate);

            // 检查所有约束
            if (!constraint_.check_constraints(box))
            {
                continue;
            }

            // 找到有效位置
            return true;
        }

        // 没有找到有效位置
        return false;
    }

    /// 计算体积利用率
    /// @return 体积利用率（0-1之间）
    double calculate_volume_rate() const
    {
        long long used_volume = 0;
        for (const auto& box : placed_boxes_)
        {
            used_volume += box.volume();
        }
        return static_cast<double>(used_volume) / container_.volume();
    }

    /// 计算重量利用率
    /// @return 重量利用率（0-1之间）
    double calculate_weight_rate() const
    {
        if (container_.load == NAN)
        {
            return NAN; // 容器未设置载重
        }

        double used_weight = 0;
        for (const auto& box : placed_boxes_)
        {
            used_weight += box.weight;
        }
        return used_weight / container_.load;
    }

    /// 选择合适的载具（目的是体积利用率尽可能高）
    /// @return 选择的载具
    Container select_container() const
    {
        // 计算剩余箱子总体积
        long long remaining_volume = 0;
        for (const auto& box : remaining_boxes_)
        {
            remaining_volume += box.volume();
        }
        // 寻找合适的载具
        for (const auto& container : containers_)
        {
            if (container.volume() >= remaining_volume)
            {
                return container;
            }
        }
        return containers_.back(); // 如果没有合适的载具，使用最后一辆（最大的）
    }

public:
    /// 构造函数
    /// @param input 输入数据
    Simple(const Input& input)
        : containers_(input.containers)
        , remaining_boxes_(input.boxes)
        , placed_boxes_()
        , container_()
        , constraint_(container_, placed_boxes_)
    {
        // 箱子按体积从大到小排序
        std::sort(remaining_boxes_.begin(), remaining_boxes_.end(), [](const auto& a, const auto& b)
                  { return a.volume() > b.volume(); });

        // 载具按体积从小到大排序
        std::sort(containers_.begin(), containers_.end(), [](const auto& a, const auto& b)
                  { return a.volume() < b.volume(); });
    }

    /// 执行装箱算法
    /// @return 装箱结果
    Output run()
    {
        Output output;

        // 逐个载具装箱，直到车辆用完或箱子装完
        while (!containers_.empty() && !remaining_boxes_.empty())
        {
            // 选择合适的载具
            container_ = select_container();
            // 清空已放置的箱子
            placed_boxes_.clear();

            // 尝试放置箱子
            for (auto& box : remaining_boxes_)
            {
                if (try_place(box))
                {
                    placed_boxes_.push_back(box);
                }
            }

            spdlog::info("Packed {} boxes in container \"{}\".", placed_boxes_.size(), container_.id);

            // 更新剩余箱子列表
            std::erase_if(remaining_boxes_, [&](const Box& box)
                          { return std::ranges::find(placed_boxes_, box) != placed_boxes_.end(); });
            // 更新可用载具列表
            containers_.erase(std::find(containers_.begin(), containers_.end(), container_));

            // 创建装箱方案
            Plan plan;
            plan.container = container_;
            plan.boxes = placed_boxes_;
            plan.volume_rate = calculate_volume_rate();
            plan.weight_rate = calculate_weight_rate();
            output.plans.push_back(plan);
        }
        // 恢复未放置箱子的位置
        for (auto& box : remaining_boxes_)
        {
            box.x = -1;
            box.y = -1;
            box.z = -1;
        }
        output.unpacked_boxes = remaining_boxes_;
        spdlog::info("Unpacked {} boxes.", output.unpacked_boxes.size());

        return output;
    }
};

#endif // ALGORITHM_H

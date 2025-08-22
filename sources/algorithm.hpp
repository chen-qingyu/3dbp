#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <algorithm>
#include <tuple>
#include <vector>

#include "entities.hpp"

/// 简单装箱算法
class Simple
{
private:
    Container container_;           // 载具
    std::vector<Box> boxes_;        // 待放置的箱子
    std::vector<Box> placed_boxes_; // 已放置的箱子

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

            // 约束1：箱子不能超出载具边界
            if (!check_bound(box))
            {
                continue;
            }

            // 约束2：箱子不能与已放置的箱子重叠
            if (!check_overlap(box))
            {
                continue;
            }

            // 约束3：箱子不能缺乏足够的支撑
            if (!check_support(box))
            {
                continue;
            }

            // 找到有效位置
            return true;
        }

        // 没有找到有效位置
        return false;
    }

    /// 检查箱子是否没有与已放置的箱子重叠
    /// @param box 待检查的箱子
    /// @return 是否没有与已放置的箱子重叠
    bool check_overlap(const Box& box) const
    {
        // 遍历所有已放置的箱子
        for (const auto& placed_box : placed_boxes_)
        {
            bool x_overlap = (box.x < placed_box.x + placed_box.lx) && (placed_box.x < box.x + box.lx);
            bool y_overlap = (box.y < placed_box.y + placed_box.ly) && (placed_box.y < box.y + box.ly);
            bool z_overlap = (box.z < placed_box.z + placed_box.lz) && (placed_box.z < box.z + box.lz);
            // 三个方向都重叠才是重叠
            if (x_overlap && y_overlap && z_overlap)
            {
                return false;
            }
        }
        return true;
    }

    /// 检查箱子是否没有超出载具边界
    /// @param box 待检查的箱子
    /// @return 是否没有超出载具边界
    bool check_bound(const Box& box) const
    {
        return (box.x + box.lx <= container_.lx &&
                box.y + box.ly <= container_.ly &&
                box.z + box.lz <= container_.lz);
    }

    /// 检查箱子是否有足够的支撑
    /// @param box 待检查的箱子
    /// @return 是否有足够的支撑
    bool check_support(const Box& box) const
    {
        // 如果箱子直接放在载具底面上，则认为有支撑
        if (box.z == 0)
        {
            return true;
        }

        // 检查所有已放置的箱子
        int actual_area = 0;
        for (const auto& support_box : placed_boxes_)
        {
            // 只考虑正好在当前箱子下方平面的箱子
            if (support_box.z + support_box.lz != box.z)
            {
                continue;
            }

            // 计算重叠的矩形区域
            int overlap_x1 = std::max(box.x, support_box.x);
            int overlap_y1 = std::max(box.y, support_box.y);
            int overlap_x2 = std::min(box.x + box.lx, support_box.x + support_box.lx);
            int overlap_y2 = std::min(box.y + box.ly, support_box.y + support_box.ly);

            if (overlap_x1 < overlap_x2 && overlap_y1 < overlap_y2)
            {
                actual_area += (overlap_x2 - overlap_x1) * (overlap_y2 - overlap_y1);
            }
        }

        // 要求底面被完全支撑
        return actual_area >= box.lx * box.ly;
    }

    /// 计算体积利用率
    /// @return 体积利用率（0-1之间）
    double calculate_volume_rate() const
    {
        long long used = 0;
        for (const auto& box : placed_boxes_)
        {
            used += box.volume();
        }
        return static_cast<double>(used) / container_.volume();
    }

public:
    /// 构造函数
    /// @param input 输入数据
    Simple(const Input& input)
        : container_(input.containers[0]) // 使用第一个载具
        , boxes_(input.boxes)
        , placed_boxes_()
    {
    }

    /// 执行装箱算法
    /// @return 装箱结果
    Output run()
    {
        // 箱子按体积从大到小排序
        std::sort(boxes_.begin(), boxes_.end(), [](const Box& a, const Box& b)
                  { return a.volume() > b.volume(); });

        // 尝试放置箱子
        for (auto& box : boxes_)
        {
            if (try_place(box))
            {
                placed_boxes_.push_back(box);
            }
        }

        // 创建装箱方案
        Plan plan;
        plan.container = container_;
        plan.boxes = placed_boxes_;
        plan.volume_rate = calculate_volume_rate();

        Output output;
        output.plans.push_back(plan);
        return output;
    }
};

#endif // ALGORITHM_H

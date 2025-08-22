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
    std::vector<Container> containers_; // 可用载具
    Container container_;               // 当前载具
    std::vector<Box> remaining_boxes_;  // 待放置箱子
    std::vector<Box> placed_boxes_;     // 已放置箱子

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
        long long used_volume = 0;
        for (const auto& box : placed_boxes_)
        {
            used_volume += box.volume();
        }
        return static_cast<double>(used_volume) / container_.volume();
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
            output.plans.push_back(plan);
        }

        return output;
    }
};

#endif // ALGORITHM_H

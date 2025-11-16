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
    std::vector<ContainerType> container_types_; // 容器类型列表
    Container container_;                        // 当前容器
    std::vector<Box> unpacked_boxes_;            // 待装载箱子
    std::vector<Box> packed_boxes_;              // 已装载箱子
    Constraint constraint_;                      // 约束检查
    Output output_;                              // 装箱结果

    /// 尝试装载箱子
    /// @param box 待装载的箱子（会被修改位置和方向）
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

        // 候选点排序，优先级：z（高度/上方）> y（宽度/后方）> x（长度/右方）
        std::ranges::sort(candidates);
        // 候选点去重
        auto [first, last] = std::ranges::unique(candidates);
        candidates.erase(first, candidates.end());

        // 尝试每个允许的方向
        for (auto orient : box.type->orients)
        {
            box.set_orient(orient);

            // 尝试每个候选位置
            for (const auto& candidate : candidates)
            {
                std::tie(box.x, box.y, box.z) = candidate;

                // 检查所有约束
                if (constraint_.check_constraints(box))
                {
                    return true; // 找到有效位置
                }
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
        return static_cast<double>(used_volume) / container_.type.volume();
    }

    /// 计算重量利用率
    /// @return 重量利用率（0-1之间）
    double calculate_weight_rate() const
    {
        if (std::isnan(container_.type.payload))
        {
            return NAN; // 容器未设置载重
        }

        double used_weight = 0;
        for (const auto& box : packed_boxes_)
        {
            used_weight += box.weight;
        }
        return used_weight / container_.type.payload;
    }

    /// 生成可用容器
    /// @return 是否成功生成容器
    bool generate_container()
    {
        // 计算剩余箱子总体积
        long long unpacked_volume = 0;
        for (const auto& box : unpacked_boxes_)
        {
            unpacked_volume += box.volume();
        }

        // 按体积从小到大查找合适的容器类型
        for (auto& type : container_types_)
        {
            if (type.quantity > 0 && type.volume() >= unpacked_volume)
            {
                type.quantity--;
                container_.type = type;
                return true;
            }
        }

        // 否则使用最大的可用容器类型
        for (auto it = container_types_.rbegin(); it != container_types_.rend(); ++it)
        {
            if (it->quantity > 0)
            {
                it->quantity--;
                container_.type = *it;
                return true;
            }
        }

        return false; // 所有容器都用完了
    }

public:
    /// 构造函数
    /// @param input 输入数据
    Algorithm(const Input& input)
        : container_types_(input.container_types)
        , container_()
        , unpacked_boxes_(input.boxes)
        , packed_boxes_()
        , constraint_(container_, packed_boxes_)
        , output_()
    {
        // 原封不动输出箱型信息
        output_.box_types = input.box_types;

        // 箱子按体积从大到小排序
        std::ranges::sort(unpacked_boxes_, [](const auto& a, const auto& b)
                          { return a.volume() > b.volume(); });

        // 容器类型按体积从小到大排序
        std::ranges::sort(container_types_, [](const auto& a, const auto& b)
                          { return a.volume() < b.volume(); });
    }

    /// 执行装箱算法
    /// @return 装箱结果
    Output run()
    {
        int container_count = 0;

        // 逐个容器装箱，直到容器用完或箱子装完
        while (!unpacked_boxes_.empty())
        {
            // 生成下一个可用容器
            if (!generate_container())
            {
                break; // 没有可用容器了
            }

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

            // 如果一个箱子都装不下，说明剩余箱子无法装载，退出循环
            if (packed_boxes_.empty())
            {
                spdlog::warn("Remaining boxes cannot be packed.");
                break;
            }

            spdlog::info("Packed {} boxes in container#{}: \"{}\".", packed_boxes_.size(), ++container_count, container_.type.id);

            // 更新剩余箱子列表
            std::erase_if(unpacked_boxes_, [&](const Box& box)
                          { return std::ranges::find(packed_boxes_, box) != packed_boxes_.end(); });

            // 创建装箱方案
            container_.boxes = packed_boxes_;
            container_.volume_rate = calculate_volume_rate();
            container_.weight_rate = calculate_weight_rate();
            output_.containers.push_back(container_);
        }
        // 重置未装载箱子的位置
        for (auto& box : unpacked_boxes_)
        {
            box.x = -1;
            box.y = -1;
            box.z = -1;
        }
        if (!unpacked_boxes_.empty())
        {
            spdlog::warn("Unpacked {} boxes.", unpacked_boxes_.size());
        }
        output_.unpacked_boxes = unpacked_boxes_;

        return output_;
    }
};

#endif // ALGORITHM_HPP

#ifndef CONSTRAINT_HPP
#define CONSTRAINT_HPP

#include <cmath>

#include "entities.hpp"

/// 约束检查类
class Constraint
{
private:
    const Container& container_;    // 当前容器的引用
    const std::vector<Box>& boxes_; // 已装载箱子的引用

public:
    Constraint(const Container& container, const std::vector<Box>& packed_boxes)
        : container_(container)
        , boxes_(packed_boxes)
    {
    }

    /// 约束1：箱子不能超出容器边界
    /// @param box 待检查的箱子
    /// @return 是否满足约束
    bool check_bound(const Box& box) const
    {
        return (box.x + box.type->lx <= container_.lx &&
                box.y + box.type->ly <= container_.ly &&
                box.z + box.type->lz <= container_.lz);
    }

    /// 约束2：箱子不能与已装载的箱子重叠
    /// @param box 待检查的箱子
    /// @return 是否满足约束
    bool check_overlap(const Box& box) const
    {
        // 遍历所有已装载的箱子
        for (const auto& packed_box : boxes_)
        {
            bool x_overlap = (box.x < packed_box.x + packed_box.type->lx) && (packed_box.x < box.x + box.type->lx);
            bool y_overlap = (box.y < packed_box.y + packed_box.type->ly) && (packed_box.y < box.y + box.type->ly);
            bool z_overlap = (box.z < packed_box.z + packed_box.type->lz) && (packed_box.z < box.z + box.type->lz);
            // 三个方向都重叠才是重叠
            if (x_overlap && y_overlap && z_overlap)
            {
                return false;
            }
        }
        return true;
    }

    /// 约束3：箱子不能缺乏足够的支撑
    /// @param box 待检查的箱子
    /// @return 是否满足约束
    bool check_support(const Box& box) const
    {
        // 如果箱子直接放在容器底面上，则认为有支撑
        if (box.z == 0)
        {
            return true;
        }

        // 检查所有已装载的箱子
        int actual_area = 0;
        for (const auto& support_box : boxes_)
        {
            // 只考虑正好在当前箱子下方平面的箱子
            if (support_box.z + support_box.type->lz != box.z)
            {
                continue;
            }

            // 计算重叠的矩形区域
            int overlap_x1 = std::max(box.x, support_box.x);
            int overlap_y1 = std::max(box.y, support_box.y);
            int overlap_x2 = std::min(box.x + box.type->lx, support_box.x + support_box.type->lx);
            int overlap_y2 = std::min(box.y + box.type->ly, support_box.y + support_box.type->ly);

            if (overlap_x1 < overlap_x2 && overlap_y1 < overlap_y2)
            {
                actual_area += (overlap_x2 - overlap_x1) * (overlap_y2 - overlap_y1);
            }
        }

        // 要求底面被完全支撑
        return actual_area >= box.type->lx * box.type->ly;
    }

    /// 约束4：箱子总重量不能超过容器载重（若输入存在载重限制）
    /// @param box 待检查的箱子
    /// @return 是否满足约束
    bool check_weight(const Box& box) const
    {
        double total = box.weight;
        for (const auto& b : boxes_)
        {
            total += b.weight;
        }
        return total <= container_.payload;
    }

    /// 检查是否满足所有约束
    /// @param box 待检查的箱子
    /// @return 是否满足所有约束
    bool check_constraints(const Box& box) const
    {
        return check_bound(box) &&
               check_overlap(box) &&
               check_support(box) &&
               (std::isnan(container_.payload) || check_weight(box));
    }
};

#endif // CONSTRAINT_HPP

#ifndef CONSTRAINT_H
#define CONSTRAINT_H

#include <cmath>

#include "entities.hpp"

class Constraint
{
private:
    const Container& container_;    // 当前载具的引用
    const std::vector<Box>& boxes_; // 已放置箱子的引用

public:
    Constraint(const Container& container, const std::vector<Box>& placed_boxes)
        : container_(container)
        , boxes_(placed_boxes)
    {
    }

    /// 约束1：箱子不能超出载具边界
    /// @param box 待检查的箱子
    /// @return 是否满足约束
    bool check_bound(const Box& box) const
    {
        return (box.x + box.lx <= container_.lx &&
                box.y + box.ly <= container_.ly &&
                box.z + box.lz <= container_.lz);
    }

    /// 约束2：箱子不能与已放置的箱子重叠
    /// @param box 待检查的箱子
    /// @return 是否满足约束
    bool check_overlap(const Box& box) const
    {
        // 遍历所有已放置的箱子
        for (const auto& placed_box : boxes_)
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

    /// 约束3：箱子不能缺乏足够的支撑
    /// @param box 待检查的箱子
    /// @return 是否满足约束
    bool check_support(const Box& box) const
    {
        // 如果箱子直接放在载具底面上，则认为有支撑
        if (box.z == 0)
        {
            return true;
        }

        // 检查所有已放置的箱子
        int actual_area = 0;
        for (const auto& support_box : boxes_)
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

    /// 约束4：箱子总重量不能超过载具载重
    bool check_weight(const Box& box) const
    {
        double total = box.weight;
        for (const auto& b : boxes_)
        {
            total += b.weight;
        }
        return total <= container_.load;
    }

    /// 检查是否满足所有约束
    /// @param box 待检查的箱子
    /// @return 是否满足所有约束
    bool check_constraints(const Box& box) const
    {
        return check_bound(box) &&
               check_overlap(box) &&
               check_support(box) &&
               (std::isnan(container_.load) || check_weight(box));
    }
};

#endif // CONSTRAINT_H

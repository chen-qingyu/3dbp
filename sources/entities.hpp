#ifndef ENTITIES_HPP
#define ENTITIES_HPP

#include <string>
#include <vector>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

/// Represents a 3D container.
struct Container
{
    std::string id; // 载具ID
    int lx, ly, lz; // 载具尺寸
    double load;    // 载具负载

    long long volume() const
    {
        return static_cast<long long>(lx) * ly * lz;
    }

    bool operator==(const Container& other) const
    {
        return this->id == other.id;
    }

    friend void from_json(const json& j, Container& c)
    {
        // 必选字段
        c.id = j["id"];
        c.lx = j["lx"];
        c.ly = j["ly"];
        c.lz = j["lz"];

        // 可选字段
        c.load = j.value("load", NAN);
    }

    friend void to_json(json& j, const Container& c)
    {
        j["id"] = c.id;
        j["lx"] = c.lx;
        j["ly"] = c.ly;
        j["lz"] = c.lz;

        if (!isnan(c.load))
        {
            j["load"] = c.load;
        }
    }
};

/// Represents a 3D box.
struct Box
{
    std::string id; // 箱子ID
    int lx, ly, lz; // 箱子尺寸
    int x, y, z;    // 箱子位置
    double weight;  // 箱子重量

    long long volume() const
    {
        return static_cast<long long>(lx) * ly * lz;
    }

    bool operator==(const Box& other) const
    {
        return this->id == other.id;
    }

    friend void from_json(const json& j, Box& b)
    {
        // 必选字段
        b.id = j["id"];
        b.lx = j["lx"];
        b.ly = j["ly"];
        b.lz = j["lz"];

        // 可选字段
        b.weight = j.value("weight", NAN);

        // 输出字段
        b.x = -1;
        b.y = -1;
        b.z = -1;
    }

    friend void to_json(json& j, const Box& b)
    {
        j["id"] = b.id;
        j["lx"] = b.lx;
        j["ly"] = b.ly;
        j["lz"] = b.lz;

        if (!isnan(b.weight))
        {
            j["weight"] = b.weight;
        }

        // 如果箱子成功装载了才输出位置信息
        if (b.x != -1 && b.y != -1 && b.z != -1)
        {
            j["x"] = b.x;
            j["y"] = b.y;
            j["z"] = b.z;
        }
    }
};

/// Represents the input data for the packing problem.
struct Input
{
    std::vector<Container> containers;
    std::vector<Box> boxes;

    friend void from_json(const json& j, Input& i)
    {
        // 必选字段
        i.containers = j["containers"];
        i.boxes = j["boxes"];
    }
};

/// Represents a packing plan.
struct Plan
{
    Container container;
    std::vector<Box> boxes;
    double volume_rate;
    double weight_rate;

    friend void to_json(json& j, const Plan& p)
    {
        j["container"] = p.container;
        j["boxes"] = p.boxes;
        j["volume_rate"] = p.volume_rate;

        if (!isnan(p.weight_rate))
        {
            j["weight_rate"] = p.weight_rate;
        }
    }
};

/// Represents the output data for the packing problem.
struct Output
{
    std::vector<Plan> plans;
    std::vector<Box> unpacked_boxes;

    friend void to_json(json& j, const Output& o)
    {
        j["plans"] = o.plans;
        j["unpacked_boxes"] = o.unpacked_boxes;
    }
};

#endif // ENTITIES_HPP

#ifndef ENTITIES_HPP
#define ENTITIES_HPP

#include <string>
#include <vector>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

/// Represents a 3D box.
struct Box
{
    // 必选字段
    std::string id; // 箱子ID
    int lx, ly, lz; // 箱子尺寸

    // 可选字段
    double weight;     // 箱子重量
    std::string group; // 箱子分组

    // 输出字段
    int x, y, z; // 箱子位置

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
        b.id = j["id"];
        b.lx = j["lx"];
        b.ly = j["ly"];
        b.lz = j["lz"];

        b.weight = j.value("weight", NAN);
        b.group = j.value("group", "");

        // 测试中反序列化可能会用到
        b.x = j.value("x", -1);
        b.y = j.value("y", -1);
        b.z = j.value("z", -1);
    }

    friend void to_json(json& j, const Box& b)
    {
        j["id"] = b.id;
        j["lx"] = b.lx;
        j["ly"] = b.ly;
        j["lz"] = b.lz;

        if (b.group != "")
        {
            j["group"] = b.group;
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

/// Represents a 3D container.
struct Container
{
    // 必选字段
    std::string id; // 容器ID
    int lx, ly, lz; // 容器尺寸

    // 可选字段
    double payload; // 容器载重

    // 输出字段
    std::vector<Box> boxes; // 装载的箱子
    double volume_rate;     // 体积利用率
    double weight_rate;     // 重量利用率

    long long volume() const
    {
        return static_cast<long long>(lx) * ly * lz;
    }

    bool operator==(const Container& other) const
    {
        return this->id == other.id && boxes == other.boxes;
    }

    friend void from_json(const json& j, Container& c)
    {
        c.id = j["id"];
        c.lx = j["lx"];
        c.ly = j["ly"];
        c.lz = j["lz"];

        c.payload = j.value("payload", NAN);

        // 测试中反序列化可能会用到
        c.boxes = j.value("boxes", std::vector<Box>{});
        c.volume_rate = j.value("volume_rate", NAN);
        c.weight_rate = j.value("weight_rate", NAN);
    }

    friend void to_json(json& j, const Container& c)
    {
        j["id"] = c.id;
        j["lx"] = c.lx;
        j["ly"] = c.ly;
        j["lz"] = c.lz;

        j["boxes"] = c.boxes;
        j["volume_rate"] = c.volume_rate;

        if (!std::isnan(c.weight_rate))
        {
            j["weight_rate"] = c.weight_rate;
        }
    }
};

/// Represents the input data for the packing problem.
struct Input
{
    // 必选字段
    std::vector<Container> containers;
    std::vector<Box> boxes;

    friend void from_json(const json& j, Input& i)
    {
        i.containers = j["containers"];
        i.boxes = j["boxes"];
    }
};

/// Represents the output data for the packing problem.
struct Output
{
    std::vector<Container> containers;
    std::vector<Box> unpacked_boxes;

    friend void to_json(json& j, const Output& o)
    {
        j["containers"] = o.containers;
        j["unpacked_boxes"] = o.unpacked_boxes;
    }
};

namespace std
{
template <>
struct hash<Box>
{
    std::size_t operator()(const Box& b) const
    {
        return std::hash<std::string>()(b.id);
    }
};

template <>
struct hash<Container>
{
    std::size_t operator()(const Container& c) const
    {
        return std::hash<std::string>()(c.id);
    }
};
} // namespace std

#endif // ENTITIES_HPP

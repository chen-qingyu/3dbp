#ifndef ENTITIES_HPP
#define ENTITIES_HPP

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

/// Represents a box type.
struct BoxType
{
    // 必选字段
    std::string id; // 箱型ID
    int lx, ly, lz; // 箱型尺寸

    bool operator==(const BoxType& other) const
    {
        return this->id == other.id;
    }

    friend void from_json(const json& j, BoxType& bt)
    {
        bt.id = j["id"];
        bt.lx = j["lx"];
        bt.ly = j["ly"];
        bt.lz = j["lz"];
    }

    friend void to_json(json& j, const BoxType& bt)
    {
        j["id"] = bt.id;
        j["lx"] = bt.lx;
        j["ly"] = bt.ly;
        j["lz"] = bt.lz;
    }
};

/// Represents a 3D box.
struct Box
{
    // 必选字段
    std::string id;      // 箱子ID
    std::string type_id; // 箱型ID

    // 可选字段
    double weight;     // 箱子重量
    std::string group; // 箱子分组

    // 输出字段
    int x, y, z; // 箱子位置

    // 内部属性
    std::shared_ptr<BoxType> type; // 箱子类型指针

    Box() = default;

    Box(std::string id, std::shared_ptr<BoxType> type, double weight, std::string group, int x, int y, int z)
        : id(std::move(id))
        , type_id(type->id)
        , weight(weight)
        , group(std::move(group))
        , x(x)
        , y(y)
        , z(z)
        , type(std::move(type))
    {
    }

    long long volume() const
    {
        return static_cast<long long>(type->lx) * type->ly * type->lz;
    }

    bool operator==(const Box& other) const
    {
        return this->id == other.id;
    }

    friend void from_json(const json& j, Box& b)
    {
        b.id = j["id"];
        b.type_id = j["type"];
        b.weight = j.value("weight", NAN);
        b.group = j.value("group", "");
    }

    friend void to_json(json& j, const Box& b)
    {
        j["id"] = b.id;
        j["type"] = b.type_id;
        j["weight"] = b.weight;
        j["group"] = b.group;

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
        return this->id == other.id;
    }

    friend void from_json(const json& j, Container& c)
    {
        c.id = j["id"];
        c.lx = j["lx"];
        c.ly = j["ly"];
        c.lz = j["lz"];
        c.payload = j.value("payload", NAN);
    }

    friend void to_json(json& j, const Container& c)
    {
        j["id"] = c.id;
        j["lx"] = c.lx;
        j["ly"] = c.ly;
        j["lz"] = c.lz;
        j["payload"] = c.payload;
        j["boxes"] = c.boxes;
        j["volume_rate"] = c.volume_rate;
        j["weight_rate"] = c.weight_rate;
    }
};

/// Represents the input data for the packing problem.
struct Input
{
    // 必选字段
    std::vector<BoxType> box_types;
    std::vector<Container> containers;
    std::vector<Box> boxes;

    friend void from_json(const json& j, Input& i)
    {
        i.box_types = j["box_types"];
        i.containers = j["containers"];
        i.boxes = j["boxes"];

        // 关联box与box_type
        std::unordered_map<std::string, std::shared_ptr<BoxType>> box_type_map;
        for (const auto& type : i.box_types)
        {
            box_type_map[type.id] = std::make_shared<BoxType>(type);
        }
        for (auto& b : i.boxes)
        {
            b.type = box_type_map[b.type_id];
        }
    }
};

/// Represents the output data for the packing problem.
struct Output
{
    std::vector<BoxType> box_types;
    std::vector<Container> containers;
    std::vector<Box> unpacked_boxes;

    friend void to_json(json& j, const Output& o)
    {
        j["box_types"] = o.box_types;
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

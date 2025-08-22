#ifndef ENTITIES_HPP
#define ENTITIES_HPP

#include <string>
#include <vector>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

/// Represents a 3D container.
struct Container
{
    std::string id;
    int lx, ly, lz;

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
    }

    friend void to_json(json& j, const Container& c)
    {
        j["id"] = c.id;
        j["lx"] = c.lx;
        j["ly"] = c.ly;
        j["lz"] = c.lz;
    }
};

/// Represents a 3D box.
struct Box
{
    std::string id;
    int lx, ly, lz;
    int x, y, z;

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
        j["x"] = b.x;
        j["y"] = b.y;
        j["z"] = b.z;
    }
};

/// Represents the input data for the packing problem.
struct Input
{
    std::vector<Container> containers;
    std::vector<Box> boxes;

    friend void from_json(const json& j, Input& i)
    {
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

    friend void to_json(json& j, const Plan& p)
    {
        j["container"] = p.container;
        j["boxes"] = p.boxes;
        j["volume_rate"] = p.volume_rate;
    }
};

/// Represents the output data for the packing problem.
struct Output
{
    std::vector<Plan> plans;

    friend void to_json(json& j, const Output& o)
    {
        j["plans"] = o.plans;
    }
};

#endif // ENTITIES_HPP

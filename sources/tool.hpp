#ifndef TOOL_HPP
#define TOOL_HPP

#include <fstream>
#include <unordered_set>

#include <nlohmann/json-schema.hpp>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include "entities.hpp"

/// 校验JSON输入数据格式
/// @param input 输入JSON对象
void validate_schema(const nlohmann::json& input)
{
    using nlohmann::json;
    try
    {
        // 加载Schema
        json schema = json::parse(std::ifstream("sources/input_schema.json"));

        // 创建校验器
        nlohmann::json_schema::json_validator validator(schema);

        // 校验数据
        validator.validate(input);
    }
    catch (const std::exception& e)
    {
        // 校验失败，退出程序
        spdlog::error("JSON validation failed: {}", e.what());
        exit(1);
    }
}

/// 校验输入数据的逻辑关系
/// @param input 输入Input对象
void validate_logic(const Input& input)
{
    // 规则1：容器id全局唯一
    std::unordered_set<std::string> container_ids;
    for (const auto& c : input.containers)
    {
        if (!container_ids.insert(c.id).second)
        {
            spdlog::error("Duplicate container id: {}.", c.id);
            exit(1);
        }
    }

    // 规则2：箱子id全局唯一
    std::unordered_set<std::string> box_ids;
    for (const auto& b : input.boxes)
    {
        if (!box_ids.insert(b.id).second)
        {
            spdlog::error("Duplicate box id: {}.", b.id);
            exit(1);
        }
    }
}

#endif // TOOL_HPP

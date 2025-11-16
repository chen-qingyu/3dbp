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
    try
    {
        auto file = std::ifstream("sources/input_schema.json");  // 读取文件
        nlohmann::json schema = nlohmann::json::parse(file);     // 加载Schema
        nlohmann::json_schema::json_validator validator(schema); // 创建校验器
        validator.validate(input);                               // 校验数据
    }
    catch (const std::exception& e) // 校验失败，退出程序
    {
        spdlog::error("JSON validation failed: {}", e.what());
        exit(1);
    }
}

/// 校验输入数据的逻辑关系
/// @param input 输入Input对象
void validate_logic(const Input& input)
{
    // 规则1：容器类型id全局唯一
    std::unordered_set<std::string> container_type_ids;
    for (const auto& ct : input.container_types)
    {
        if (!container_type_ids.insert(ct.id).second)
        {
            spdlog::error("Duplicate container type id: {}.", ct.id);
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

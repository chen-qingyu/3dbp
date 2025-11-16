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
static void validate_schema(const nlohmann::json& input)
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
static void validate_logic(const Input& input)
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

/// 解析JSON输入文件并校验
/// @param input_file 输入文件路径
/// @return Input对象
Input parse_input(const std::string& input_file)
{
    // 读取文件
    std::ifstream file(input_file);
    if (!file.is_open())
    {
        spdlog::error("Failed to open input file \"{}\".", input_file);
        exit(1);
    }
    nlohmann::json input_json = nlohmann::json::parse(file);
    file.close();

    // 校验输入
    validate_schema(input_json);
    Input input_obj = input_json;
    validate_logic(input_obj);
    spdlog::info("Successfully validated input \"{}\".", input_file);

    return input_obj;
}

#endif // TOOL_HPP

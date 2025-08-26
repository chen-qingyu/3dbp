#ifndef TOOL_HPP
#define TOOL_HPP

#include <fstream>

#include <nlohmann/json-schema.hpp>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

/// 校验JSON输入数据
/// @param input JSON输入数据
/// @return 校验通过后的原始输入
auto validate(const nlohmann::json& input)
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

        // 校验通过，返回原始输入
        return input;
    }
    catch (const std::exception& e)
    {
        // 校验失败，退出程序
        spdlog::error("JSON validation failed: {}", e.what());
        exit(0);
    }
}

#endif // TOOL_HPP

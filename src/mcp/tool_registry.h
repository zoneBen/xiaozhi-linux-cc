#pragma once

#include <string>
#include <map>
#include <vector>
#include <functional>
#include <unordered_map>
#include "xiaozhi_types.h"

namespace xiaozhi {

struct ToolParameter {
    std::string name;
    std::string type;  // "string", "number", "boolean"
    std::string description;
    bool required = false;
};

struct ToolDefinition {
    std::string name;
    std::string description;
    std::vector<ToolParameter> parameters;
    std::function<std::string(const std::map<std::string, std::string>&)> handler;
};

class ToolRegistry {
public:
    ToolRegistry();
    ~ToolRegistry();

    // 注册工具
    void registerTool(const std::string& name, 
                     const std::string& description,
                     const std::vector<ToolParameter>& parameters,
                     std::function<std::string(const std::map<std::string, std::string>&)> handler);

    // 调用工具
    std::string callTool(const std::string& name, 
                        const std::map<std::string, std::string>& arguments);

    // 获取所有工具
    std::vector<ToolDefinition> getAllTools() const;

    // 检查工具是否存在
    bool hasTool(const std::string& name) const;

private:
    std::unordered_map<std::string, ToolDefinition> tools_;
};

} // namespace xiaozhi
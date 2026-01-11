#include "tool_registry.h"
#include <iostream>

namespace xiaozhi {

ToolRegistry::ToolRegistry() {
    std::cout << "[ToolRegistry] 初始化工具注册表" << std::endl;
}

ToolRegistry::~ToolRegistry() {
    std::cout << "[ToolRegistry] 工具注册表已销毁" << std::endl;
}

void ToolRegistry::registerTool(const std::string& name, 
                              const std::string& description,
                              const std::vector<ToolParameter>& parameters,
                              std::function<std::string(const std::map<std::string, std::string>&)> handler) {
    ToolDefinition tool;
    tool.name = name;
    tool.description = description;
    tool.parameters = parameters;
    tool.handler = handler;
    
    tools_[name] = tool;
    
    std::cout << "[ToolRegistry] 已注册工具: " << name << std::endl;
}

std::string ToolRegistry::callTool(const std::string& name, 
                                  const std::map<std::string, std::string>& arguments) {
    auto it = tools_.find(name);
    if (it != tools_.end()) {
        // 验证参数
        for (const auto& param : it->second.parameters) {
            if (param.required && arguments.find(param.name) == arguments.end()) {
                return "{\"error\":\"Missing required parameter: " + param.name + "\"}";
            }
        }
        
        // 调用工具处理函数
        try {
            std::string result = it->second.handler(arguments);
            return result;
        } catch (const std::exception& e) {
            return "{\"error\":\"Tool execution failed: " + std::string(e.what()) + "\"}";
        }
    } else {
        return "{\"error\":\"Tool not found: " + name + "\"}";
    }
}

std::vector<ToolDefinition> ToolRegistry::getAllTools() const {
    std::vector<ToolDefinition> tool_list;
    for (const auto& pair : tools_) {
        tool_list.push_back(pair.second);
    }
    return tool_list;
}

bool ToolRegistry::hasTool(const std::string& name) const {
    return tools_.find(name) != tools_.end();
}

} // namespace xiaozhi
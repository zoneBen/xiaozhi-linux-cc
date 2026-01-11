#include "mcp_server.h"
#include <iostream>
#include <json-c/json.h>
#include <sstream>
#include <thread>

namespace xiaozhi {

McpServer::McpServer(int port) : port_(port) {
    std::cout << "[McpServer] 初始化MCP服务器，端口: " << port << std::endl;
    
    // 注册内置工具
    addTool("system.info", 
            "获取系统信息", 
            {}, 
            [](const std::map<std::string, std::string>& params) -> std::string {
                return "{\"os\":\"Linux\",\"version\":\"1.0.0\",\"uptime\":12345}";
            });
            
    addTool("device.status", 
            "获取设备状态", 
            {}, 
            [](const std::map<std::string, std::string>& params) -> std::string {
                return "{\"cpu_usage\":25,\"memory_usage\":60,\"temperature\":45}";
            });
}

McpServer::~McpServer() {
    stop();
    if (server_thread_.joinable()) {
        server_thread_.join();
    }
    std::cout << "[McpServer] MCP服务器已销毁" << std::endl;
}

bool McpServer::start() {
    if (!running_) {
        running_ = true;
        server_thread_ = std::thread(&McpServer::serverLoop, this);
        std::cout << "[McpServer] MCP服务器已启动，端口: " << port_ << std::endl;
        return true;
    }
    return false;
}

void McpServer::stop() {
    if (running_) {
        running_ = false;
        std::cout << "[McpServer] MCP服务器已停止" << std::endl;
    }
}

bool McpServer::isRunning() const {
    return running_;
}

void McpServer::addTool(const std::string& name, 
                        const std::string& description,
                        const std::vector<ToolParameter>& parameters,
                        std::function<std::string(const std::map<std::string, std::string>&)> handler) {
    std::lock_guard<std::mutex> lock(tools_mutex_);
    
    ToolDefinition tool;
    tool.name = name;
    tool.description = description;
    tool.parameters = parameters;
    tool.handler = handler;
    
    tools_[name] = tool;
    
    std::cout << "[McpServer] 已注册工具: " << name << std::endl;
}

std::string McpServer::handleRequest(const std::string& request) {
    // 解析JSON请求
    json_object* jobj = json_tokener_parse(request.c_str());
    if (!jobj) {
        std::cerr << "[McpServer] 错误: 无效的JSON请求" << std::endl;
        return "{\"error\":\"Invalid JSON\"}";
    }

    std::string method;
    json_object* method_obj = nullptr;
    if (json_object_object_get_ex(jobj, "method", &method_obj)) {
        method = json_object_get_string(method_obj);
    }

    std::string response;
    if (method == "initialize") {
        json_object* params_obj = nullptr;
        if (json_object_object_get_ex(jobj, "params", &params_obj)) {
            response = handleInitialize(json_object_get_string(params_obj));
        } else {
            response = handleInitialize("{}");
        }
    } else if (method == "tools/list") {
        response = handleListTools();
    } else if (method == "tools/call") {
        json_object* params_obj = nullptr;
        if (json_object_object_get_ex(jobj, "params", &params_obj)) {
            response = handleCallTool(json_object_get_string(params_obj));
        } else {
            response = "{\"error\":\"Missing params\"}";
        }
    } else {
        response = "{\"error\":\"Unknown method\"}";
    }

    json_object_put(jobj); // 释放内存
    return response;
}

void McpServer::process() {
    // 处理MCP相关任务
    // 在实际实现中，这里会处理MCP协议事件
}

void McpServer::serverLoop() {
    // 模拟服务器主循环
    while (running_) {
        // 在实际实现中，这里会监听和处理MCP请求
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

std::string McpServer::handleInitialize(const std::string& params) {
    std::ostringstream response;
    response << "{"
             << "\"id\":\"" << "session_" << rand() << "\","
             << "\"capabilities\":{"
             << "\"tools\":true,"
             << "\"execution\":true"
             << "}"
             << "}";
    return response.str();
}

std::string McpServer::handleListTools() {
    std::ostringstream response;
    response << "{"
             << "\"tools\":[";
    
    {
        std::lock_guard<std::mutex> lock(tools_mutex_);
        bool first = true;
        for (const auto& tool_pair : tools_) {
            const auto& tool = tool_pair.second;
            if (!first) response << ",";
            first = false;
            
            response << "{"
                     << "\"name\":\"" << tool.name << "\","
                     << "\"description\":\"" << tool.description << "\","
                     << "\"inputSchema\":{"
                     << "\"type\":\"object\","
                     << "\"properties\":{";
            
            bool param_first = true;
            for (const auto& param : tool.parameters) {
                if (!param_first) response << ",";
                param_first = false;
                
                response << "\"" << param.name << "\":{"
                         << "\"type\":\"" << param.type << "\","
                         << "\"description\":\"" << param.description << "\""
                         << "}";
            }
            
            response << "},"
                     << "\"required\":[";
            
            bool req_first = true;
            for (const auto& param : tool.parameters) {
                if (param.required) {
                    if (!req_first) response << ",";
                    req_first = false;
                    response << "\"" << param.name << "\"";
                }
            }
            
            response << "]"
                     << "}"
                     << "}";
        }
    }
    
    response << "],"
             << "\"totalTools\":" << tools_.size()
             << "}";
    
    return response.str();
}

std::string McpServer::handleCallTool(const std::string& params) {
    // 解析调用参数
    json_object* jobj = json_tokener_parse(params.c_str());
    if (!jobj) {
        return "{\"error\":\"Invalid params JSON\"}";
    }
    
    json_object* name_obj = nullptr;
    json_object* args_obj = nullptr;
    
    if (!json_object_object_get_ex(jobj, "name", &name_obj) ||
        !json_object_object_get_ex(jobj, "arguments", &args_obj)) {
        json_object_put(jobj);
        return "{\"error\":\"Missing name or arguments\"}";
    }
    
    std::string tool_name = json_object_get_string(name_obj);
    
    // 将参数对象转换为map
    std::map<std::string, std::string> args_map;
    json_object_object_foreach(args_obj, key, val) {
        args_map[key] = json_object_get_string(val);
    }
    
    // 查找并执行工具
    {
        std::lock_guard<std::mutex> lock(tools_mutex_);
        auto it = tools_.find(tool_name);
        if (it != tools_.end()) {
            std::string result = it->second.handler(args_map);
            std::ostringstream response;
            response << "{"
                     << "\"result\":" << result << ","
                     << "\"isError\":false"
                     << "}";
            json_object_put(jobj);
            return response.str();
        }
    }
    
    json_object_put(jobj);
    return "{\"error\":\"Tool not found\"}";
}

} // namespace xiaozhi
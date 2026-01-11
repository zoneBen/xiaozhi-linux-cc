#pragma once

#include <string>
#include <map>
#include <functional>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
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

class McpServer {
public:
    McpServer(int port = 8080);
    ~McpServer();

    bool start();
    void stop();
    bool isRunning() const;

    // 注册工具
    void addTool(const std::string& name, 
                 const std::string& description,
                 const std::vector<ToolParameter>& parameters,
                 std::function<std::string(const std::map<std::string, std::string>&)> handler);

    // 处理MCP请求
    std::string handleRequest(const std::string& request);

    void process();

private:
    int port_;
    std::atomic<bool> running_{false};

    std::map<std::string, ToolDefinition> tools_;
    std::thread server_thread_;
    std::mutex tools_mutex_;

    void serverLoop();
    
    // 内部处理方法
    std::string handleInitialize(const std::string& params);
    std::string handleListTools();
    std::string handleCallTool(const std::string& params);
};

} // namespace xiaozhi
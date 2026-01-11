#pragma once

#include <string>
#include "tool_registry.h"

// Forward declaration
struct json_object;

namespace xiaozhi {

class JsonRpcHandler {
public:
    explicit JsonRpcHandler(ToolRegistry& registry);
    ~JsonRpcHandler();

    // 处理JSON-RPC 2.0请求
    std::string handleRequest(const std::string& request);

private:
    ToolRegistry& tool_registry_;

    // 请求处理方法
    std::string handleInitialize(json_object* request);
    std::string handleListTools(json_object* request);
    std::string handleCallTool(json_object* request);

    // 辅助方法
    std::string createErrorResponse(int code, const std::string& message, json_object* id_obj);
};

} // namespace xiaozhi
#pragma once

#include <string>
#include <functional>
#include "xiaozhi_types.h"

namespace xiaozhi {

class ProtocolHandler {
public:
    ProtocolHandler();
    ~ProtocolHandler();

    // 处理WebSocket消息
    std::string handleWebSocketMessage(const std::string& message);

    // 设置响应回调
    void setResponseCallback(std::function<void(const std::string&)> callback);

private:
    std::function<void(const std::string&)> response_callback_;

    // 消息处理方法
    std::string handleHello(const std::string& message);
    std::string handleListen(const std::string& message);
    std::string handleStt(const std::string& message);
    std::string handleTts(const std::string& message);
    std::string handleMcp(const std::string& message);
    
    // 辅助方法
    std::string createErrorMessage(const std::string& error_msg);
};

} // namespace xiaozhi
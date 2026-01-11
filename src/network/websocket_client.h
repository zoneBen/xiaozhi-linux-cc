#pragma once

#include <string>
#include <thread>
#include <mutex>
#include <atomic>
#include <functional>
#include "xiaozhi_types.h"

namespace xiaozhi {

class WebsocketClient {
public:
    WebsocketClient();
    ~WebsocketClient();

    bool connect(const std::string& url);
    void disconnect();
    bool isConnected() const;

    // 发送文本消息
    bool sendText(const std::string& message);
    
    // 发送二进制数据（如音频）
    bool sendBinary(const std::vector<uint8_t>& data);

    void process();

    // 设置消息回调
    void setTextMessageCallback(std::function<void(const std::string&)> callback);
    void setBinaryMessageCallback(std::function<void(const std::vector<uint8_t>&)> callback);
    void setConnectionStatusCallback(std::function<void(bool connected)> callback);

private:
    std::atomic<bool> connected_{false};
    std::atomic<bool> should_stop_{false};

    std::string server_url_;
    
    std::function<void(const std::string&)> text_message_callback_;
    std::function<void(const std::vector<uint8_t>&)> binary_message_callback_;
    std::function<void(bool)> connection_status_callback_;

    std::thread client_thread_;
    std::mutex socket_mutex_;

    void clientLoop();
};

} // namespace xiaozhi
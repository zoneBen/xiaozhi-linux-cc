#include "websocket_client.h"
#include <iostream>
#include <chrono>
#include <thread>

namespace xiaozhi {

WebsocketClient::WebsocketClient() {
    std::cout << "[WebsocketClient] 初始化WebSocket客户端" << std::endl;
}

WebsocketClient::~WebsocketClient() {
    disconnect();
    should_stop_ = true;
    if (client_thread_.joinable()) {
        client_thread_.join();
    }
    std::cout << "[WebsocketClient] WebSocket客户端已销毁" << std::endl;
}

bool WebsocketClient::connect(const std::string& url) {
    server_url_ = url;
    
    // 模拟连接过程
    std::cout << "[WebsocketClient] 正在连接到服务器: " << url << std::endl;
    
    // 启动客户端线程
    if (!client_thread_.joinable()) {
        client_thread_ = std::thread(&WebsocketClient::clientLoop, this);
    }
    
    // 模拟连接成功
    connected_ = true;
    
    if (connection_status_callback_) {
        connection_status_callback_(true);
    }
    
    std::cout << "[WebsocketClient] 已连接到服务器" << std::endl;
    return true;
}

void WebsocketClient::disconnect() {
    if (connected_) {
        connected_ = false;
        std::cout << "[WebsocketClient] 断开服务器连接" << std::endl;
        
        if (connection_status_callback_) {
            connection_status_callback_(false);
        }
    }
}

bool WebsocketClient::isConnected() const {
    return connected_;
}

bool WebsocketClient::sendText(const std::string& message) {
    if (!connected_) {
        std::cerr << "[WebsocketClient] 错误: 未连接到服务器" << std::endl;
        return false;
    }

    // 在实际实现中，这里会通过WebSocket发送文本消息
    std::cout << "[WebsocketClient] 发送文本消息: " << message.substr(0, 50) 
              << (message.length() > 50 ? "..." : "") << std::endl;
    
    return true;
}

bool WebsocketClient::sendBinary(const std::vector<uint8_t>& data) {
    if (!connected_) {
        std::cerr << "[WebsocketClient] 错误: 未连接到服务器" << std::endl;
        return false;
    }

    // 在实际实现中，这里会通过WebSocket发送二进制数据
    std::cout << "[WebsocketClient] 发送二进制数据，大小: " << data.size() << " 字节" << std::endl;
    
    return true;
}

void WebsocketClient::process() {
    // 处理网络相关任务
    // 在实际实现中，这里会处理网络事件和消息
}

void WebsocketClient::setTextMessageCallback(std::function<void(const std::string&)> callback) {
    text_message_callback_ = callback;
}

void WebsocketClient::setBinaryMessageCallback(std::function<void(const std::vector<uint8_t>&)> callback) {
    binary_message_callback_ = callback;
}

void WebsocketClient::setConnectionStatusCallback(std::function<void(bool connected)> callback) {
    connection_status_callback_ = callback;
}

void WebsocketClient::clientLoop() {
    // 模拟客户端主循环
    while (!should_stop_) {
        if (connected_) {
            // 模拟接收消息
            // 在实际实现中，这里会从WebSocket连接接收消息
            
            // 模拟一些网络活动
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        } else {
            // 如果断开连接，等待一段时间再尝试
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }
}

} // namespace xiaozhi
#include "mqtt_client.h"
#include <iostream>
#include <thread>
#include <chrono>

namespace xiaozhi {

MqttClient::MqttClient() : connected_(false), should_stop_(false) {
    std::cout << "[MqttClient] 初始化MQTT客户端" << std::endl;
}

MqttClient::~MqttClient() {
    disconnect();
    should_stop_ = true;
    if (client_thread_.joinable()) {
        client_thread_.join();
    }
    std::cout << "[MqttClient] MQTT客户端已销毁" << std::endl;
}

bool MqttClient::connect(const std::string& broker_url, const std::string& client_id) {
    broker_url_ = broker_url;
    client_id_ = client_id;
    
    std::cout << "[MqttClient] 正在连接到MQTT代理: " << broker_url << std::endl;
    
    // 启动客户端线程
    if (!client_thread_.joinable()) {
        client_thread_ = std::thread(&MqttClient::clientLoop, this);
    }
    
    // 模拟连接成功
    connected_ = true;
    
    if (connection_status_callback_) {
        connection_status_callback_(true);
    }
    
    std::cout << "[MqttClient] 已连接到MQTT代理" << std::endl;
    return true;
}

void MqttClient::disconnect() {
    if (connected_) {
        connected_ = false;
        std::cout << "[MqttClient] 断开MQTT代理连接" << std::endl;
        
        if (connection_status_callback_) {
            connection_status_callback_(false);
        }
    }
}

bool MqttClient::isConnected() const {
    return connected_;
}

bool MqttClient::publish(const std::string& topic, const std::string& message) {
    if (!connected_) {
        std::cerr << "[MqttClient] 错误: 未连接到MQTT代理" << std::endl;
        return false;
    }

    std::cout << "[MqttClient] 发布消息到主题 '" << topic << "': " << message.substr(0, 50) 
              << (message.length() > 50 ? "..." : "") << std::endl;
    
    return true;
}

bool MqttClient::subscribe(const std::string& topic) {
    if (!connected_) {
        std::cerr << "[MqttClient] 错误: 未连接到MQTT代理" << std::endl;
        return false;
    }

    subscriptions_.insert(topic);
    std::cout << "[MqttClient] 订阅主题: " << topic << std::endl;
    
    return true;
}

void MqttClient::process() {
    // 处理MQTT相关任务
}

void MqttClient::setMessageCallback(std::function<void(const std::string&, const std::string&)> callback) {
    message_callback_ = callback;
}

void MqttClient::setConnectionStatusCallback(std::function<void(bool connected)> callback) {
    connection_status_callback_ = callback;
}

void MqttClient::clientLoop() {
    // 模拟客户端主循环
    while (!should_stop_) {
        if (connected_) {
            // 模拟接收消息
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        } else {
            // 如果断开连接，等待一段时间再尝试
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }
}

} // namespace xiaozhi
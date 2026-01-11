#pragma once

#include <string>
#include <thread>
#include <mutex>
#include <atomic>
#include <functional>
#include <set>
#include "xiaozhi_types.h"

namespace xiaozhi {

class MqttClient {
public:
    MqttClient();
    ~MqttClient();

    bool connect(const std::string& broker_url, const std::string& client_id = "");
    void disconnect();
    bool isConnected() const;

    // 发布和订阅
    bool publish(const std::string& topic, const std::string& message);
    bool subscribe(const std::string& topic);

    void process();

    // 设置回调
    void setMessageCallback(std::function<void(const std::string&, const std::string&)> callback);
    void setConnectionStatusCallback(std::function<void(bool connected)> callback);

private:
    std::atomic<bool> connected_{false};
    std::atomic<bool> should_stop_{false};

    std::string broker_url_;
    std::string client_id_;
    
    std::function<void(const std::string&, const std::string&)> message_callback_;
    std::function<void(bool)> connection_status_callback_;
    
    std::set<std::string> subscriptions_;

    std::thread client_thread_;
    std::mutex socket_mutex_;

    void clientLoop();
};

} // namespace xiaozhi
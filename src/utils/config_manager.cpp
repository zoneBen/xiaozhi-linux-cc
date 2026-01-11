#include "config_manager.h"
#include <iostream>
#include <fstream>
#include <json-c/json.h>

namespace xiaozhi {

ConfigManager& ConfigManager::getInstance() {
    static ConfigManager instance;
    return instance;
}

bool ConfigManager::loadConfig(const std::string& filepath) {
    std::string path = filepath.empty() ? config_path_ : filepath;
    if (path.empty()) {
        path = "~/.config/xiaozhi/config.json"; // 默认路径
    }

    std::cout << "[ConfigManager] 加载配置文件: " << path << std::endl;

    // 读取文件内容
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "[ConfigManager] 错误: 无法打开配置文件: " << path << std::endl;
        
        // 使用默认配置
        server_config_.websocket_url = "wss://api.xiaozhi.example.com/ws";
        server_config_.mqtt_broker = "mqtt://broker.xiaozhi.example.com:1883";
        server_config_.auth_token = "your_auth_token_here";
        
        audio_config_.input_device = "default";
        audio_config_.output_device = "default";
        audio_config_.sample_rate = 16000;
        audio_config_.channels = 1;
        audio_config_.opus_bitrate = 32000;
        
        mcp_config_.enabled = true;
        mcp_config_.port = 8080;
        
        network_config_.reconnect_interval = 5;
        network_config_.timeout = 30;
        
        std::cout << "[ConfigManager] 使用默认配置" << std::endl;
        return false;
    }

    // 读取整个文件内容
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();

    // 解析JSON
    json_object* root = json_tokener_parse(content.c_str());
    if (!root) {
        std::cerr << "[ConfigManager] 错误: 无法解析JSON配置文件" << std::endl;
        return false;
    }

    // 解析服务器配置
    json_object* server_obj = nullptr;
    if (json_object_object_get_ex(root, "server", &server_obj)) {
        json_object* url_obj = nullptr;
        if (json_object_object_get_ex(server_obj, "websocket_url", &url_obj)) {
            server_config_.websocket_url = json_object_get_string(url_obj);
        }
        
        json_object* mqtt_obj = nullptr;
        if (json_object_object_get_ex(server_obj, "mqtt_broker", &mqtt_obj)) {
            server_config_.mqtt_broker = json_object_get_string(mqtt_obj);
        }
        
        json_object* token_obj = nullptr;
        if (json_object_object_get_ex(server_obj, "auth_token", &token_obj)) {
            server_config_.auth_token = json_object_get_string(token_obj);
        }
    }

    // 解析音频配置
    json_object* audio_obj = nullptr;
    if (json_object_object_get_ex(root, "audio", &audio_obj)) {
        json_object* input_dev_obj = nullptr;
        if (json_object_object_get_ex(audio_obj, "input_device", &input_dev_obj)) {
            audio_config_.input_device = json_object_get_string(input_dev_obj);
        }
        
        json_object* output_dev_obj = nullptr;
        if (json_object_object_get_ex(audio_obj, "output_device", &output_dev_obj)) {
            audio_config_.output_device = json_object_get_string(output_dev_obj);
        }
        
        json_object* sample_rate_obj = nullptr;
        if (json_object_object_get_ex(audio_obj, "sample_rate", &sample_rate_obj)) {
            audio_config_.sample_rate = json_object_get_int(sample_rate_obj);
        }
        
        json_object* channels_obj = nullptr;
        if (json_object_object_get_ex(audio_obj, "channels", &channels_obj)) {
            audio_config_.channels = json_object_get_int(channels_obj);
        }
        
        json_object* bitrate_obj = nullptr;
        if (json_object_object_get_ex(audio_obj, "opus_bitrate", &bitrate_obj)) {
            audio_config_.opus_bitrate = json_object_get_int(bitrate_obj);
        }
    }

    // 解析MCP配置
    json_object* mcp_obj = nullptr;
    if (json_object_object_get_ex(root, "mcp", &mcp_obj)) {
        json_object* enabled_obj = nullptr;
        if (json_object_object_get_ex(mcp_obj, "enabled", &enabled_obj)) {
            mcp_config_.enabled = json_object_get_boolean(enabled_obj);
        }
        
        json_object* port_obj = nullptr;
        if (json_object_object_get_ex(mcp_obj, "port", &port_obj)) {
            mcp_config_.port = json_object_get_int(port_obj);
        }
    }

    // 解析网络配置
    json_object* network_obj = nullptr;
    if (json_object_object_get_ex(root, "network", &network_obj)) {
        json_object* interval_obj = nullptr;
        if (json_object_object_get_ex(network_obj, "reconnect_interval", &interval_obj)) {
            network_config_.reconnect_interval = json_object_get_int(interval_obj);
        }
        
        json_object* timeout_obj = nullptr;
        if (json_object_object_get_ex(network_obj, "timeout", &timeout_obj)) {
            network_config_.timeout = json_object_get_int(timeout_obj);
        }
    }

    json_object_put(root); // 释放内存
    config_path_ = path;

    std::cout << "[ConfigManager] 配置加载成功" << std::endl;
    return true;
}

bool ConfigManager::saveConfig(const std::string& filepath) {
    std::string path = filepath.empty() ? config_path_ : filepath;
    if (path.empty()) {
        path = "~/.config/xiaozhi/config.json"; // 默认路径
    }

    std::cout << "[ConfigManager] 保存配置文件: " << path << std::endl;

    // 创建JSON对象
    json_object* root = json_object_new_object();

    // 服务器配置
    json_object* server_obj = json_object_new_object();
    json_object_object_add(server_obj, "websocket_url", 
                          json_object_new_string(server_config_.websocket_url.c_str()));
    json_object_object_add(server_obj, "mqtt_broker", 
                          json_object_new_string(server_config_.mqtt_broker.c_str()));
    json_object_object_add(server_obj, "auth_token", 
                          json_object_new_string(server_config_.auth_token.c_str()));
    json_object_object_add(root, "server", server_obj);

    // 音频配置
    json_object* audio_obj = json_object_new_object();
    json_object_object_add(audio_obj, "input_device", 
                          json_object_new_string(audio_config_.input_device.c_str()));
    json_object_object_add(audio_obj, "output_device", 
                          json_object_new_string(audio_config_.output_device.c_str()));
    json_object_object_add(audio_obj, "sample_rate", 
                          json_object_new_int(audio_config_.sample_rate));
    json_object_object_add(audio_obj, "channels", 
                          json_object_new_int(audio_config_.channels));
    json_object_object_add(audio_obj, "opus_bitrate", 
                          json_object_new_int(audio_config_.opus_bitrate));
    json_object_object_add(root, "audio", audio_obj);

    // MCP配置
    json_object* mcp_obj = json_object_new_object();
    json_object_object_add(mcp_obj, "enabled", 
                          json_object_new_boolean(mcp_config_.enabled));
    json_object_object_add(mcp_obj, "port", 
                          json_object_new_int(mcp_config_.port));
    json_object_object_add(root, "mcp", mcp_obj);

    // 网络配置
    json_object* network_obj = json_object_new_object();
    json_object_object_add(network_obj, "reconnect_interval", 
                          json_object_new_int(network_config_.reconnect_interval));
    json_object_object_add(network_obj, "timeout", 
                          json_object_new_int(network_config_.timeout));
    json_object_object_add(root, "network", network_obj);

    // 写入文件
    std::ofstream file(path);
    if (!file.is_open()) {
        std::cerr << "[ConfigManager] 错误: 无法创建配置文件: " << path << std::endl;
        json_object_put(root);
        return false;
    }

    file << json_object_to_json_string_ext(root, JSON_C_TO_STRING_PRETTY);
    file.close();

    json_object_put(root); // 释放内存
    config_path_ = path;

    std::cout << "[ConfigManager] 配置保存成功" << std::endl;
    return true;
}

} // namespace xiaozhi
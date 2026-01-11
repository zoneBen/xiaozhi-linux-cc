#pragma once

#include <string>
#include <map>
#include "xiaozhi_types.h"

namespace xiaozhi {

struct ServerConfig {
    std::string websocket_url;
    std::string mqtt_broker;
    std::string auth_token;
};

struct AudioConfig {
    std::string input_device;
    std::string output_device;
    int sample_rate;
    int channels;
    int opus_bitrate;
};

struct McpConfig {
    bool enabled;
    int port;
};

struct NetworkConfig {
    int reconnect_interval;
    int timeout;
};

class ConfigManager {
public:
    static ConfigManager& getInstance();

    bool loadConfig(const std::string& filepath = "");
    bool saveConfig(const std::string& filepath = "");

    // 获取配置
    ServerConfig getServerConfig() const { return server_config_; }
    AudioConfig getAudioConfig() const { return audio_config_; }
    McpConfig getMcpConfig() const { return mcp_config_; }
    NetworkConfig getNetworkConfig() const { return network_config_; }

    // 设置配置
    void setServerConfig(const ServerConfig& config) { server_config_ = config; }
    void setAudioConfig(const AudioConfig& config) { audio_config_ = config; }
    void setMcpConfig(const McpConfig& config) { mcp_config_ = config; }
    void setNetworkConfig(const NetworkConfig& config) { network_config_ = config; }

private:
    ConfigManager() = default;
    ~ConfigManager() = default;

    ServerConfig server_config_;
    AudioConfig audio_config_;
    McpConfig mcp_config_;
    NetworkConfig network_config_;

    std::string config_path_;
};

} // namespace xiaozhi
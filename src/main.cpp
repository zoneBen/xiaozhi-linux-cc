#include <iostream>
#include <thread>
#include <chrono>
#include "audio/audio_manager.h"
#include "network/websocket_client.h"
#include "mcp/mcp_server.h"
#include "ai/ai_engine.h"
#include "utils/config_manager.h"
#include "utils/logger.h"

int main(int argc, char *argv[]) {
    // 初始化日志系统
    xiaozhi::Logger::getInstance().info("小智AI - Linux版启动中...");
    
    // 加载配置
    auto& configMgr = xiaozhi::ConfigManager::getInstance();
    configMgr.loadConfig();
    
    auto serverConfig = configMgr.getServerConfig();
    auto audioConfig = configMgr.getAudioConfig();
    auto mcpConfig = configMgr.getMcpConfig();
    auto networkConfig = configMgr.getNetworkConfig();
    
    // 初始化音频管理器
    xiaozhi::AudioManager audioManager;
    audioManager.initialize(audioConfig.sample_rate, audioConfig.channels);
    
    // 初始化网络客户端
    xiaozhi::WebsocketClient wsClient;
    
    // 设置网络事件回调
    wsClient.setConnectionStatusCallback([&](bool connected) {
        if (connected) {
            xiaozhi::Logger::getInstance().info("WebSocket连接已建立");
        } else {
            xiaozhi::Logger::getInstance().warn("WebSocket连接已断开");
        }
    });
    
    // 初始化MCP服务器
    xiaozhi::McpServer mcpServer(mcpConfig.port);
    if (mcpConfig.enabled) {
        mcpServer.start();
    }
    
    // 初始化AI引擎
    xiaozhi::AiEngine aiEngine;
    aiEngine.initialize();
    aiEngine.start();
    
    xiaozhi::Logger::getInstance().info("小智AI - Linux版已启动");
    
    // 连接到WebSocket服务器
    wsClient.connect(serverConfig.websocket_url);
    
    // 主循环
    bool running = true;
    while(running) {
        // 处理事件循环
        audioManager.process();
        wsClient.process();
        mcpServer.process();
        aiEngine.process();
        
        // 短暂休眠避免CPU占用过高
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        
        // 检查退出条件（例如，可以监听信号或其他退出机制）
        // 这里简化为简单的退出条件
        static int counter = 0;
        if (++counter > 10000) { // 100秒后退出，实际应用中应使用信号处理
            running = false;
        }
    }
    
    // 清理资源
    wsClient.disconnect();
    if (mcpConfig.enabled) {
        mcpServer.stop();
    }
    aiEngine.stop();
    
    xiaozhi::Logger::getInstance().info("小智AI - Linux版已退出");
    
    return 0;
}
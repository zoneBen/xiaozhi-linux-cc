#include "conversation_manager.h"
#include <iostream>
#include <sstream>
#include <ctime>

namespace xiaozhi {

ConversationManager::ConversationManager() {
    std::cout << "[ConversationManager] 初始化对话管理器" << std::endl;
}

ConversationManager::~ConversationManager() {
    std::cout << "[ConversationManager] 对话管理器已销毁" << std::endl;
}

void ConversationManager::startNewSession(const std::string& session_id) {
    sessions_[session_id] = {};
    current_session_ = session_id;
    std::cout << "[ConversationManager] 开始新的对话会话: " << session_id << std::endl;
}

void ConversationManager::endSession(const std::string& session_id) {
    auto it = sessions_.find(session_id);
    if (it != sessions_.end()) {
        sessions_.erase(it);
        if (current_session_ == session_id) {
            current_session_ = "";
        }
        std::cout << "[ConversationManager] 结束对话会话: " << session_id << std::endl;
    }
}

std::string ConversationManager::processInput(const std::string& input_text) {
    if (current_session_.empty()) {
        startNewSession(generateSessionId());
    }
    
    // 添加用户输入到对话历史
    addMessageToHistory(current_session_, "user", input_text);
    
    // 生成AI回复
    std::string ai_response = generateResponse(input_text);
    
    // 添加AI回复到对话历史
    addMessageToHistory(current_session_, "assistant", ai_response);
    
    std::cout << "[ConversationManager] 处理输入: " << input_text.substr(0, 50) 
              << (input_text.length() > 50 ? "..." : "") << std::endl;
    
    return ai_response;
}

std::vector<Message> ConversationManager::getConversationHistory(const std::string& session_id) {
    auto it = sessions_.find(session_id);
    if (it != sessions_.end()) {
        return it->second;
    }
    return {};
}

void ConversationManager::clearHistory(const std::string& session_id) {
    auto it = sessions_.find(session_id);
    if (it != sessions_.end()) {
        it->second.clear();
        std::cout << "[ConversationManager] 已清除会话历史: " << session_id << std::endl;
    }
}

std::string ConversationManager::generateSessionId() {
    auto now = std::time(nullptr);
    char buffer[20];
    std::strftime(buffer, sizeof(buffer), "%Y%m%d_%H%M%S", std::localtime(&now));
    
    std::stringstream ss;
    ss << "sess_" << buffer << "_" << rand();
    return ss.str();
}

void ConversationManager::addMessageToHistory(const std::string& session_id, 
                                           const std::string& role, 
                                           const std::string& content) {
    auto it = sessions_.find(session_id);
    if (it != sessions_.end()) {
        Message msg;
        msg.role = role;
        msg.content = content;
        msg.timestamp = std::time(nullptr);
        
        it->second.push_back(msg);
    }
}

std::string ConversationManager::generateResponse(const std::string& input) {
    // 模拟AI响应生成
    std::string response = "AI回复: 收到您的消息 \"" + input + "\"，这是模拟的AI回复内容。";
    
    // 模拟一些简单的对话逻辑
    if (input.find("你好") != std::string::npos || input.find("hello") != std::string::npos) {
        response = "您好！很高兴见到您。有什么我可以帮您的吗？";
    } else if (input.find("再见") != std::string::npos || input.find("bye") != std::string::npos) {
        response = "再见！期待下次与您交谈。";
    } else if (input.find("名字") != std::string::npos) {
        response = "我是小智AI，一个智能语音助手。";
    } else if (input.find("帮助") != std::string::npos || input.find("help") != std::string::npos) {
        response = "我可以帮您回答问题、提供信息查询等服务。请告诉我您需要什么帮助。";
    }
    
    return response;
}

} // namespace xiaozhi
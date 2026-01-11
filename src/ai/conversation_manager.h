#pragma once

#include <string>
#include <vector>
#include <map>
#include <ctime>

namespace xiaozhi {

struct Message {
    std::string role;      // "user", "assistant", "system"
    std::string content;
    std::time_t timestamp;
};

class ConversationManager {
public:
    ConversationManager();
    ~ConversationManager();

    // 会话管理
    void startNewSession(const std::string& session_id = "");
    void endSession(const std::string& session_id);

    // 对话处理
    std::string processInput(const std::string& input_text);

    // 获取对话历史
    std::vector<Message> getConversationHistory(const std::string& session_id);

    // 清除对话历史
    void clearHistory(const std::string& session_id);

private:
    std::map<std::string, std::vector<Message>> sessions_;
    std::string current_session_;

    // 辅助方法
    std::string generateSessionId();
    void addMessageToHistory(const std::string& session_id, 
                           const std::string& role, 
                           const std::string& content);
    std::string generateResponse(const std::string& input);
};

} // namespace xiaozhi
#pragma once

#include <string>
#include <functional>
#include <map>
#include "xiaozhi_types.h"

namespace xiaozhi {

class CommandHandler {
public:
    CommandHandler();
    ~CommandHandler();

    // 注册命令处理器
    void registerCommand(const std::string& name, 
                        const std::string& description,
                        std::function<void(const std::vector<std::string>&)> handler);

    // 处理命令
    void handleCommand(const std::string& command);

    // 显示帮助信息
    void showHelp() const;

    // 启动交互式命令行
    void startInteractiveMode();

private:
    struct CommandInfo {
        std::string description;
        std::function<void(const std::vector<std::string>&)> handler;
    };

    std::map<std::string, CommandInfo> commands_;
    
    // 内置命令
    void registerBuiltInCommands();
    void handleTestAudio(const std::vector<std::string>& args);
    void handleVoiceMode(const std::vector<std::string>& args);
    void handleTextMode(const std::vector<std::string>& args);
    void handleStatus(const std::vector<std::string>& args);
};

} // namespace xiaozhi
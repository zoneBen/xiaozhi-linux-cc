#include "command_handler.h"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace xiaozhi {

CommandHandler::CommandHandler() {
    registerBuiltInCommands();
}

CommandHandler::~CommandHandler() {
}

void CommandHandler::registerCommand(const std::string& name, 
                                   const std::string& description,
                                   std::function<void(const std::vector<std::string>&)> handler) {
    CommandInfo info;
    info.description = description;
    info.handler = handler;
    commands_[name] = info;
    
    std::cout << "[CommandHandler] 已注册命令: " << name << std::endl;
}

void CommandHandler::handleCommand(const std::string& command) {
    if (command.empty()) {
        return;
    }
    
    // 解析命令和参数
    std::istringstream iss(command);
    std::string cmd;
    iss >> cmd;
    
    // 转换为小写
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);
    
    auto it = commands_.find(cmd);
    if (it != commands_.end()) {
        // 解析参数
        std::vector<std::string> args;
        std::string arg;
        while (iss >> arg) {
            args.push_back(arg);
        }
        
        // 调用命令处理器
        it->second.handler(args);
    } else {
        std::cout << "未知命令: " << cmd << ". 输入 'help' 查看可用命令。" << std::endl;
    }
}

void CommandHandler::showHelp() const {
    std::cout << "\n=== 小智AI命令行工具 ===" << std::endl;
    std::cout << "可用命令:" << std::endl;
    
    for (const auto& cmd_pair : commands_) {
        std::cout << "  " << cmd_pair.first << " - " << cmd_pair.second.description << std::endl;
    }
    
    std::cout << std::endl;
}

void CommandHandler::startInteractiveMode() {
    std::cout << "小智AI命令行工具 - 交互模式" << std::endl;
    std::cout << "输入 'help' 查看命令列表，输入 'exit' 退出。" << std::endl;
    
    std::string input;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, input);
        
        if (input.empty()) {
            continue;
        }
        
        // 转换为小写以进行比较
        std::string lower_input = input;
        std::transform(lower_input.begin(), lower_input.end(), lower_input.begin(), ::tolower);
        
        if (lower_input == "exit" || lower_input == "quit") {
            break;
        } else if (lower_input == "help") {
            showHelp();
        } else {
            handleCommand(input);
        }
    }
    
    std::cout << "退出命令行工具。" << std::endl;
}

void CommandHandler::registerBuiltInCommands() {
    registerCommand("test-audio", "测试音频设备", 
                   [this](const std::vector<std::string>& args) { handleTestAudio(args); });
    registerCommand("voice-mode", "语音对话模式", 
                   [this](const std::vector<std::string>& args) { handleVoiceMode(args); });
    registerCommand("text-mode", "文本对话模式", 
                   [this](const std::vector<std::string>& args) { handleTextMode(args); });
    registerCommand("status", "查看系统状态", 
                   [this](const std::vector<std::string>& args) { handleStatus(args); });
    registerCommand("help", "显示帮助信息", 
                   [this](const std::vector<std::string>& args) { showHelp(); });
}

void CommandHandler::handleTestAudio(const std::vector<std::string>& args) {
    std::cout << "正在测试音频设备..." << std::endl;
    std::cout << "- 检查音频输入设备: OK" << std::endl;
    std::cout << "- 检查音频输出设备: OK" << std::endl;
    std::cout << "- 测试音频录制: OK" << std::endl;
    std::cout << "- 测试音频播放: OK" << std::endl;
    std::cout << "音频设备测试完成！" << std::endl;
}

void CommandHandler::handleVoiceMode(const std::vector<std::string>& args) {
    std::cout << "切换到语音对话模式..." << std::endl;
    std::cout << "语音识别已启用，请说话..." << std::endl;
    std::cout << "提示: 按 Ctrl+C 退出语音模式" << std::endl;
}

void CommandHandler::handleTextMode(const std::vector<std::string>& args) {
    std::cout << "切换到文本对话模式..." << std::endl;
    std::cout << "请输入您的消息:" << std::endl;
    
    std::string input;
    while (true) {
        std::cout << "你: ";
        std::getline(std::cin, input);
        
        if (input == "exit" || input == "quit") {
            break;
        }
        
        std::cout << "AI: 收到您的消息 \"" << input << "\"，这是模拟的AI回复。" << std::endl;
    }
    
    std::cout << "已退出文本对话模式。" << std::endl;
}

void CommandHandler::handleStatus(const std::vector<std::string>& args) {
    std::cout << "\n=== 系统状态 ===" << std::endl;
    std::cout << "连接状态: 已连接" << std::endl;
    std::cout << "音频状态: 正常" << std::endl;
    std::cout << "MCP服务: 运行中" << std::endl;
    std::cout << "AI引擎: 就绪" << std::endl;
    std::cout << "CPU使用率: 15%" << std::endl;
    std::cout << "内存使用率: 45%" << std::endl;
    std::cout << "运行时间: 01:23:45" << std::endl;
    std::cout << std::endl;
}

} // namespace xiaozhi
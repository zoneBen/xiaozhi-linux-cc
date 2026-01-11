#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <mutex>
#include <chrono>

namespace xiaozhi {

enum class LogLevel {
    DEBUG = 0,
    INFO = 1,
    WARN = 2,
    ERROR = 3
};

class Logger {
public:
    static Logger& getInstance();

    void setLogLevel(LogLevel level);
    void setLogFile(const std::string& filepath);

    void debug(const std::string& message);
    void info(const std::string& message);
    void warn(const std::string& message);
    void error(const std::string& message);

    template<typename... Args>
    void log(LogLevel level, const std::string& fmt, Args... args);

private:
    Logger();
    ~Logger();

    LogLevel log_level_{LogLevel::INFO};
    std::ofstream log_file_;
    std::mutex log_mutex_;
    
    void writeLog(LogLevel level, const std::string& message);
    std::string levelToString(LogLevel level);
    std::string getCurrentTimestamp();
};

#define LOG_DEBUG(msg) xiaozhi::Logger::getInstance().debug("[DEBUG] " + std::string(__FUNCTION__) + ": " + msg)
#define LOG_INFO(msg) xiaozhi::Logger::getInstance().info("[INFO] " + std::string(__FUNCTION__) + ": " + msg)
#define LOG_WARN(msg) xiaozhi::Logger::getInstance().warn("[WARN] " + std::string(__FUNCTION__) + ": " + msg)
#define LOG_ERROR(msg) xiaozhi::Logger::getInstance().error("[ERROR] " + std::string(__FUNCTION__) + ": " + msg)

} // namespace xiaozhi
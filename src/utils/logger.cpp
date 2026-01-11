#include "logger.h"
#include <iomanip>
#include <sstream>
#include <cstdarg>
#include <cstdlib>

namespace xiaozhi {

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

Logger::Logger() {
    // 检查环境变量设置日志级别
    const char* log_level = std::getenv("XIAOZHI_LOG_LEVEL");
    if (log_level) {
        std::string level_str(log_level);
        if (level_str == "DEBUG") {
            log_level_ = LogLevel::DEBUG;
        } else if (level_str == "INFO") {
            log_level_ = LogLevel::INFO;
        } else if (level_str == "WARN") {
            log_level_ = LogLevel::WARN;
        } else if (level_str == "ERROR") {
            log_level_ = LogLevel::ERROR;
        }
    }
    
    // 设置默认日志文件
    setLogFile("~/.local/share/xiaozhi/logs/xiaozhi.log");
}

Logger::~Logger() {
    if (log_file_.is_open()) {
        log_file_.close();
    }
}

void Logger::setLogLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(log_mutex_);
    log_level_ = level;
}

void Logger::setLogFile(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(log_mutex_);
    
    if (log_file_.is_open()) {
        log_file_.close();
    }
    
    // 创建目录（如果不存在）
    std::string dir_path = filepath.substr(0, filepath.find_last_of('/'));
    (void)system(("mkdir -p " + dir_path).c_str()); // 忽略返回值以避免警告
    
    log_file_.open(filepath, std::ios::app);
    if (!log_file_.is_open()) {
        std::cerr << "无法打开日志文件: " << filepath << std::endl;
    }
}

void Logger::debug(const std::string& message) {
    if (log_level_ <= LogLevel::DEBUG) {
        writeLog(LogLevel::DEBUG, message);
    }
}

void Logger::info(const std::string& message) {
    if (log_level_ <= LogLevel::INFO) {
        writeLog(LogLevel::INFO, message);
    }
}

void Logger::warn(const std::string& message) {
    if (log_level_ <= LogLevel::WARN) {
        writeLog(LogLevel::WARN, message);
    }
}

void Logger::error(const std::string& message) {
    if (log_level_ <= LogLevel::ERROR) {
        writeLog(LogLevel::ERROR, message);
    }
}

void Logger::writeLog(LogLevel level, const std::string& message) {
    std::lock_guard<std::mutex> lock(log_mutex_);
    
    std::string timestamp = getCurrentTimestamp();
    std::string level_str = levelToString(level);
    
    std::string log_line = timestamp + " [" + level_str + "] " + message + "\n";
    
    // 输出到控制台
    if (level >= LogLevel::WARN) {
        std::cerr << log_line;
    } else {
        std::cout << log_line;
    }
    
    // 写入日志文件
    if (log_file_.is_open()) {
        log_file_ << log_line;
        log_file_.flush();
    }
}

std::string Logger::levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARN: return "WARN";
        case LogLevel::ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

std::string Logger::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

    char buffer[100];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&time_t));
    
    std::ostringstream oss;
    oss << buffer << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return oss.str();
}

} // namespace xiaozhi
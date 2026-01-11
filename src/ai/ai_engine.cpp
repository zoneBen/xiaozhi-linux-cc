#include "ai_engine.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <sstream>

namespace xiaozhi {

AiEngine::AiEngine() {
    std::cout << "[AiEngine] 初始化AI引擎" << std::endl;
}

AiEngine::~AiEngine() {
    stop();
    if (engine_thread_.joinable()) {
        engine_thread_.join();
    }
    std::cout << "[AiEngine] AI引擎已销毁" << std::endl;
}

bool AiEngine::initialize() {
    if (!initialized_) {
        initialized_ = true;
        std::cout << "[AiEngine] AI引擎初始化完成" << std::endl;
        return true;
    }
    return false;
}

void AiEngine::start() {
    if (initialized_ && !running_) {
        running_ = true;
        engine_thread_ = std::thread(&AiEngine::engineLoop, this);
        std::cout << "[AiEngine] AI引擎已启动" << std::endl;
    }
}

void AiEngine::stop() {
    if (running_) {
        running_ = false;
        std::cout << "[AiEngine] AI引擎已停止" << std::endl;
    }
}

bool AiEngine::isRunning() const {
    return running_;
}

bool AiEngine::sendTextMessage(const std::string& message) {
    if (!initialized_) {
        std::cerr << "[AiEngine] 错误: AI引擎未初始化" << std::endl;
        return false;
    }

    {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        text_messages_.push(message);
    }
    
    std::cout << "[AiEngine] 已接收文本消息: " << message.substr(0, 50) 
              << (message.length() > 50 ? "..." : "") << std::endl;
    
    return true;
}

bool AiEngine::sendAudioInput(const AudioData& audio_data) {
    if (!initialized_) {
        std::cerr << "[AiEngine] 错误: AI引擎未初始化" << std::endl;
        return false;
    }

    {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        audio_inputs_.push(audio_data);
    }
    
    std::cout << "[AiEngine] 已接收音频输入，大小: " << audio_data.size() << " 采样点" << std::endl;
    
    return true;
}

void AiEngine::setTextResponseCallback(std::function<void(const std::string&)> callback) {
    text_response_callback_ = callback;
}

void AiEngine::setAudioResponseCallback(std::function<void(AudioData&)> callback) {
    audio_response_callback_ = callback;
}

void AiEngine::process() {
    // 处理AI相关任务
    // 在实际实现中，这里会处理AI推理和响应
}

void AiEngine::engineLoop() {
    // 模拟AI引擎主循环
    while (running_) {
        // 处理文本消息队列
        {
            std::lock_guard<std::mutex> lock(queue_mutex_);
            while (!text_messages_.empty()) {
                std::string message = text_messages_.front();
                text_messages_.pop();
                
                // 处理文本请求
                std::string response = processTextRequest(message);
                
                // 发送文本响应
                if (text_response_callback_) {
                    text_response_callback_(response);
                }
            }
            
            // 处理音频输入队列
            while (!audio_inputs_.empty()) {
                AudioData audio_data = audio_inputs_.front();
                audio_inputs_.pop();
                
                // 在实际实现中，这里会进行语音识别
                std::string recognized_text = "模拟语音识别结果: 这是用户语音的内容";
                
                // 处理识别结果
                std::string response = processTextRequest(recognized_text);
                
                // 发送文本响应
                if (text_response_callback_) {
                    text_response_callback_(response);
                }
                
                // 如果有音频响应回调，也发送音频
                if (audio_response_callback_) {
                    AudioData audio_response = processAudioResponse(response);
                    audio_response_callback_(audio_response);
                }
            }
        }
        
        // 模拟处理间隔
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

std::string AiEngine::processTextRequest(const std::string& text) {
    // 模拟AI处理文本请求
    std::ostringstream response;
    response << "AI回复: 收到您的消息 \"" << text << "\"，这是模拟的AI回复内容。";
    
    std::cout << "[AiEngine] 处理文本请求，生成回复" << std::endl;
    return response.str();
}

AudioData AiEngine::processAudioResponse(const std::string& text) {
    // 模拟将文本转换为音频数据
    AudioData audio_data(1600); // 模拟80ms的音频数据
    for (size_t i = 0; i < audio_data.size(); ++i) {
        // 生成模拟音频波形
        audio_data[i] = static_cast<int16_t>(32767.0 * 0.3 * sin(2 * M_PI * 440 * i / 16000));
    }
    
    std::cout << "[AiEngine] 生成音频响应数据" << std::endl;
    return audio_data;
}

} // namespace xiaozhi
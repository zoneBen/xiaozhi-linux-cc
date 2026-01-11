#pragma once

#include <string>
#include <thread>
#include <mutex>
#include <atomic>
#include <functional>
#include <queue>
#include "xiaozhi_types.h"

namespace xiaozhi {

class AiEngine {
public:
    AiEngine();
    ~AiEngine();

    bool initialize();
    void start();
    void stop();
    bool isRunning() const;

    // 发送文本消息给AI
    bool sendTextMessage(const std::string& message);
    
    // 发送音频数据给AI（语音识别输入）
    bool sendAudioInput(const AudioData& audio_data);

    // 设置AI响应回调
    void setTextResponseCallback(std::function<void(const std::string&)> callback);
    void setAudioResponseCallback(std::function<void(AudioData&)> callback);

    void process();

private:
    std::atomic<bool> initialized_{false};
    std::atomic<bool> running_{false};

    std::function<void(const std::string&)> text_response_callback_;
    std::function<void(AudioData&)> audio_response_callback_;

    std::queue<std::string> text_messages_;
    std::queue<AudioData> audio_inputs_;
    
    std::thread engine_thread_;
    std::mutex queue_mutex_;

    void engineLoop();
    
    // 内部处理方法
    std::string processTextRequest(const std::string& text);
    AudioData processAudioResponse(const std::string& text);
};

} // namespace xiaozhi
// 通用类型定义
#pragma once

#include <string>
#include <vector>
#include <functional>
#include <memory>

namespace xiaozhi {

// 音频采样率类型
enum class SampleRate {
    SR_8000 = 8000,
    SR_16000 = 16000,
    SR_24000 = 24000,
    SR_48000 = 48000
};

// 音频通道数
enum class Channels {
    MONO = 1,
    STEREO = 2
};

// 音频数据类型
using AudioData = std::vector<int16_t>;

// 消息回调类型
using MessageCallback = std::function<void(const std::string&)>;

// 音频数据回调类型
using AudioCallback = std::function<void(const AudioData&)>;

} // namespace xiaozhi

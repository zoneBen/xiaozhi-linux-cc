#include "audio_manager.h"
#include <iostream>
#include <chrono>
#include <cstring>
#include <alsa/asoundlib.h>

namespace xiaozhi {

AudioManager::AudioManager() : sample_rate_(16000), channels_(1) {
    std::cout << "[AudioManager] 初始化音频管理器" << std::endl;
}

AudioManager::~AudioManager() {
    stopRecording();
    stopPlayback();
    if (record_thread_.joinable()) {
        record_thread_.join();
    }
    if (play_thread_.joinable()) {
        play_thread_.join();
    }
    std::cout << "[AudioManager] 音频管理器已销毁" << std::endl;
}

bool AudioManager::initialize(int sample_rate, int channels) {
    sample_rate_ = sample_rate;
    channels_ = channels;

    // 这里可以添加具体的音频设备初始化代码
    std::cout << "[AudioManager] 音频系统初始化完成 (采样率: " << sample_rate 
              << ", 通道数: " << channels << ")" << std::endl;
    
    initialized_ = true;
    return true;
}

void AudioManager::startRecording() {
    if (!initialized_) {
        std::cerr << "[AudioManager] 错误: 音频系统未初始化" << std::endl;
        return;
    }

    if (!recording_) {
        recording_ = true;
        record_thread_ = std::thread(&AudioManager::recordLoop, this);
        std::cout << "[AudioManager] 开始录音" << std::endl;
    }
}

void AudioManager::stopRecording() {
    if (recording_) {
        recording_ = false;
        if (record_thread_.joinable()) {
            record_thread_.join();
        }
        std::cout << "[AudioManager] 停止录音" << std::endl;
    }
}

void AudioManager::startPlayback() {
    if (!initialized_) {
        std::cerr << "[AudioManager] 错误: 音频系统未初始化" << std::endl;
        return;
    }

    if (!playing_) {
        playing_ = true;
        play_thread_ = std::thread(&AudioManager::playLoop, this);
        std::cout << "[AudioManager] 开始播放" << std::endl;
    }
}

void AudioManager::stopPlayback() {
    if (playing_) {
        playing_ = false;
        if (play_thread_.joinable()) {
            play_thread_.join();
        }
        std::cout << "[AudioManager] 停止播放" << std::endl;
    }
}

void AudioManager::process() {
    // 处理音频相关任务
    // 在实际实现中，这里会处理音频数据流
}

void AudioManager::setRecordCallback(std::function<void(const AudioData&)> callback) {
    record_callback_ = callback;
}

void AudioManager::setPlaybackCallback(std::function<void(AudioData&)> callback) {
    playback_callback_ = callback;
}

void AudioManager::recordLoop() {
    // 模拟录音循环
    while (recording_) {
        // 在实际实现中，这里会从音频设备读取数据
        // 并调用record_callback_传递音频数据
        
        // 模拟音频数据
        AudioData audio_data(320); // 模拟60ms的16kHz单声道数据
        for (size_t i = 0; i < audio_data.size(); ++i) {
            audio_data[i] = static_cast<int16_t>(i % 1000); // 模拟音频数据
        }
        
        if (record_callback_) {
            record_callback_(audio_data);
        }
        
        // 模拟录音间隔
        std::this_thread::sleep_for(std::chrono::milliseconds(60));
    }
}

void AudioManager::playLoop() {
    // 模拟播放循环
    while (playing_) {
        // 在实际实现中，这里会从playback_callback_获取数据
        // 并将其发送到音频设备进行播放
        
        AudioData audio_data;
        if (playback_callback_) {
            playback_callback_(audio_data);
            // 实际播放逻辑会在callback中处理
        }
        
        // 模拟播放间隔
        std::this_thread::sleep_for(std::chrono::milliseconds(60));
    }
}

} // namespace xiaozhi
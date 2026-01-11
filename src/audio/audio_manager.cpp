#include "audio_manager.h"
#include "alsa_handler.h"
#include "opus_encoder.h"
#include "opus_decoder.h"
#include <iostream>
#include <chrono>
#include <cstring>
#include <thread>
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
    
    // 初始化ALSA处理器
    alsa_handler_ = std::make_unique<AlsaHandler>();
    if (!alsa_handler_->initialize(sample_rate, channels)) {
        std::cerr << "[AudioManager] ALSA处理器初始化失败" << std::endl;
        return false;
    }
    
    // 初始化Opus编码器
    opus_encoder_ = std::make_unique<OpusEncoder>();
    if (!opus_encoder_->initialize(sample_rate, channels, 32000)) {
        std::cerr << "[AudioManager] Opus编码器初始化失败" << std::endl;
        return false;
    }
    
    // 初始化Opus解码器
    opus_decoder_ = std::make_unique<OpusDecoder>();
    if (!opus_decoder_->initialize(sample_rate, channels)) {
        std::cerr << "[AudioManager] Opus解码器初始化失败" << std::endl;
        return false;
    }

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
    if (!alsa_handler_) {
        std::cerr << "[AudioManager] 错误: ALSA处理器未初始化" << std::endl;
        return;
    }
    
    // 启动录音
    alsa_handler_->startCapture();
    
    // 实际录音循环
    while (recording_) {
        AudioData audio_data;
        // 从ALSA设备读取音频数据
        if (alsa_handler_->readAudioData(audio_data, 320)) { // 读取60ms数据
            if (record_callback_) {
                record_callback_(audio_data);
            }
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    
    // 停止录音
    alsa_handler_->stopCapture();
}

void AudioManager::playLoop() {
    if (!alsa_handler_) {
        std::cerr << "[AudioManager] 错误: ALSA处理器未初始化" << std::endl;
        return;
    }
    
    // 启动播放
    alsa_handler_->startPlayback();
    
    // 实际播放循环
    while (playing_) {
        AudioData audio_data;
        if (playback_callback_) {
            playback_callback_(audio_data);
            
            // 将音频数据发送到ALSA设备进行播放
            if (!audio_data.empty()) {
                alsa_handler_->writeAudioData(audio_data);
            }
        } else {
            // 如果没有回调，短暂休眠
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    
    // 停止播放
    alsa_handler_->stopPlayback();
}

} // namespace xiaozhi
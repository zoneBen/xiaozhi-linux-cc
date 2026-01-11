#include "opus_decoder.h"
#include <iostream>
#include <opus/opus.h>

namespace xiaozhi {

OpusDecoder::OpusDecoder() : decoder_(nullptr), initialized_(false) {
    std::cout << "[OpusDecoder] 初始化Opus解码器" << std::endl;
}

OpusDecoder::~OpusDecoder() {
    cleanup();
    std::cout << "[OpusDecoder] Opus解码器已销毁" << std::endl;
}

bool OpusDecoder::initialize(int sample_rate, int channels) {
    if (initialized_) {
        cleanup();
    }
    
    int error;
    decoder_ = opus_decoder_create(sample_rate, channels, &error);
    if (error != OPUS_OK) {
        std::cerr << "[OpusDecoder] 创建解码器失败: " << error << std::endl;
        return false;
    }
    
    sample_rate_ = sample_rate;
    channels_ = channels;
    
    initialized_ = true;
    std::cout << "[OpusDecoder] 解码器初始化完成 (采样率: " << sample_rate 
              << ", 通道数: " << channels << ")" << std::endl;
    
    return true;
}

AudioData OpusDecoder::decode(const std::vector<uint8_t>& opus_data) {
    if (!initialized_ || !decoder_ || opus_data.empty()) {
        return {};
    }
    
    // 假设输出帧大小为20ms (sample_rate * 0.02)
    int frame_size = sample_rate_ * 20 / 1000;
    AudioData pcm_data(frame_size * channels_);
    
    int decoded_samples = opus_decode(decoder_, 
                                    opus_data.data(), 
                                    opus_data.size(), 
                                    reinterpret_cast<opus_int16*>(pcm_data.data()), 
                                    frame_size, 
                                    0);
    
    if (decoded_samples < 0) {
        std::cerr << "[OpusDecoder] 解码失败: " << decoded_samples << std::endl;
        return {};
    }
    
    pcm_data.resize(decoded_samples * channels_);
    return pcm_data;
}

void OpusDecoder::cleanup() {
    if (decoder_) {
        opus_decoder_destroy(decoder_);
        decoder_ = nullptr;
    }
    initialized_ = false;
}

} // namespace xiaozhi
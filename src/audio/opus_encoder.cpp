#include "opus_encoder.h"
#include <iostream>
#include <opus/opus.h>
#include <opus/opusenc.h>

namespace xiaozhi {

OpusEncoder::OpusEncoder() : encoder_(nullptr), initialized_(false) {
    std::cout << "[OpusEncoder] 初始化Opus编码器" << std::endl;
}

OpusEncoder::~OpusEncoder() {
    cleanup();
    std::cout << "[OpusEncoder] Opus编码器已销毁" << std::endl;
}

bool OpusEncoder::initialize(int sample_rate, int channels, int bitrate) {
    if (initialized_) {
        cleanup();
    }
    
    int error;
    encoder_ = opus_encoder_create(sample_rate, channels, OPUS_APPLICATION_AUDIO, &error);
    if (error != OPUS_OK) {
        std::cerr << "[OpusEncoder] 创建编码器失败: " << error << std::endl;
        return false;
    }
    
    // 设置编码参数
    opus_encoder_ctl(encoder_, OPUS_SET_BITRATE(bitrate));
    opus_encoder_ctl(encoder_, OPUS_SET_VBR(1)); // 启用可变比特率
    opus_encoder_ctl(encoder_, OPUS_SET_DTX(1)); // 启用静音检测
    
    sample_rate_ = sample_rate;
    channels_ = channels;
    bitrate_ = bitrate;
    
    initialized_ = true;
    std::cout << "[OpusEncoder] 编码器初始化完成 (采样率: " << sample_rate 
              << ", 通道数: " << channels << ", 比特率: " << bitrate << ")" << std::endl;
    
    return true;
}

std::vector<uint8_t> OpusEncoder::encode(const AudioData& pcm_data) {
    if (!initialized_ || !encoder_ || pcm_data.empty()) {
        return {};
    }
    
    // Opus帧大小计算 (最大120ms)
    int frame_size = sample_rate_ / 100; // 10ms帧
    std::vector<uint8_t> encoded_data(frame_size * 2); // 预分配空间
    
    int encoded_len = opus_encode(encoder_, 
                                 reinterpret_cast<const opus_int16*>(pcm_data.data()), 
                                 frame_size, 
                                 encoded_data.data(), 
                                 encoded_data.size());
    
    if (encoded_len < 0) {
        std::cerr << "[OpusEncoder] 编码失败: " << encoded_len << std::endl;
        return {};
    }
    
    encoded_data.resize(encoded_len);
    return encoded_data;
}

void OpusEncoder::cleanup() {
    if (encoder_) {
        opus_encoder_destroy(encoder_);
        encoder_ = nullptr;
    }
    initialized_ = false;
}

} // namespace xiaozhi
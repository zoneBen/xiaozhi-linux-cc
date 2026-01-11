#include "alsa_handler.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/time.h>

namespace xiaozhi {

AlsaHandler::AlsaHandler() : input_handle_(nullptr), output_handle_(nullptr) {
    std::cout << "[AlsaHandler] 初始化ALSA音频处理器" << std::endl;
}

AlsaHandler::~AlsaHandler() {
    closeAudioDevices();
    std::cout << "[AlsaHandler] ALSA音频处理器已销毁" << std::endl;
}

bool AlsaHandler::initialize(int sample_rate, int channels, const std::string& device_name) {
    sample_rate_ = sample_rate;
    channels_ = channels;
    device_name_ = device_name.empty() ? "default" : device_name;
    
    if (!openAudioDevices()) {
        std::cerr << "[AlsaHandler] 错误: 无法打开音频设备: " << device_name_ << std::endl;
        return false;
    }
    
    std::cout << "[AlsaHandler] 音频设备初始化成功 (采样率: " << sample_rate_ 
              << ", 通道数: " << channels_ << ", 设备: " << device_name_ << ")" << std::endl;
    
    return true;
}

bool AlsaHandler::openAudioDevices() {
    int err;
    
    // 打开录音设备
    if ((err = snd_pcm_open(&input_handle_, device_name_.c_str(), SND_PCM_STREAM_CAPTURE, 0)) < 0) {
        std::cerr << "[AlsaHandler] 无法打开录音设备: " << snd_strerror(err) << std::endl;
        return false;
    }
    
    if (!configureAudioParams(input_handle_, true)) {
        std::cerr << "[AlsaHandler] 无法配置录音设备参数" << std::endl;
        snd_pcm_close(input_handle_);
        input_handle_ = nullptr;
        return false;
    }
    
    // 打开播放设备
    if ((err = snd_pcm_open(&output_handle_, device_name_.c_str(), SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
        std::cerr << "[AlsaHandler] 无法打开播放设备: " << snd_strerror(err) << std::endl;
        if (input_handle_) {
            snd_pcm_close(input_handle_);
            input_handle_ = nullptr;
        }
        return false;
    }
    
    if (!configureAudioParams(output_handle_, false)) {
        std::cerr << "[AlsaHandler] 无法配置播放设备参数" << std::endl;
        snd_pcm_close(input_handle_);
        snd_pcm_close(output_handle_);
        input_handle_ = nullptr;
        output_handle_ = nullptr;
        return false;
    }
    
    return true;
}

bool AlsaHandler::configureAudioParams(snd_pcm_t* handle, bool is_capture) {
    snd_pcm_hw_params_t* params;
    int err;
    
    snd_pcm_hw_params_alloca(&params);
    
    if ((err = snd_pcm_hw_params_any(handle, params)) < 0) {
        std::cerr << "[AlsaHandler] 无法初始化硬件参数: " << snd_strerror(err) << std::endl;
        return false;
    }
    
    // 设置访问类型
    if ((err = snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
        std::cerr << "[AlsaHandler] 无法设置访问类型: " << snd_strerror(err) << std::endl;
        return false;
    }
    
    // 设置样本格式
    if ((err = snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE)) < 0) {
        std::cerr << "[AlsaHandler] 无法设置样本格式: " << snd_strerror(err) << std::endl;
        return false;
    }
    
    // 设置采样率
    unsigned int actual_rate = sample_rate_;
    int dir = 0;
    if ((err = snd_pcm_hw_params_set_rate_near(handle, params, &actual_rate, &dir)) < 0) {
        std::cerr << "[AlsaHandler] 无法设置采样率: " << snd_strerror(err) << std::endl;
        return false;
    }
    
    if (actual_rate != sample_rate_) {
        std::cout << "[AlsaHandler] 注意: 期望采样率 " << sample_rate_ 
                  << "Hz，实际获得 " << actual_rate << "Hz" << std::endl;
    }
    
    // 设置通道数
    if ((err = snd_pcm_hw_params_set_channels(handle, params, channels_)) < 0) {
        std::cerr << "[AlsaHandler] 无法设置通道数: " << snd_strerror(err) << std::endl;
        return false;
    }
    
    // 应用硬件参数
    if ((err = snd_pcm_hw_params(handle, params)) < 0) {
        std::cerr << "[AlsaHandler] 无法应用硬件参数: " << snd_strerror(err) << std::endl;
        return false;
    }
    
    // 设置周期大小
    snd_pcm_sw_params_t* sw_params;
    snd_pcm_sw_params_alloca(&sw_params);
    snd_pcm_sw_params_current(handle, sw_params);
    
    snd_pcm_uframes_t period_size = sample_rate_ / 100; // 约10ms的周期
    if ((err = snd_pcm_sw_params_set_start_threshold(handle, sw_params, period_size)) < 0) {
        std::cerr << "[AlsaHandler] 无法设置启动阈值: " << snd_strerror(err) << std::endl;
        return false;
    }
    
    if ((err = snd_pcm_sw_params(handle, sw_params)) < 0) {
        std::cerr << "[AlsaHandler] 无法应用软件参数: " << snd_strerror(err) << std::endl;
        return false;
    }
    
    return true;
}

bool AlsaHandler::readAudioData(AudioData& audio_data, size_t frames) {
    if (!input_handle_) {
        return false;
    }
    
    audio_data.resize(frames * channels_);
    snd_pcm_sframes_t result;
    
    result = snd_pcm_readi(input_handle_, audio_data.data(), frames);
    
    if (result == -EPIPE) {
        // 缓冲区溢出，恢复
        snd_pcm_recover(input_handle_, result, 0);
        result = snd_pcm_readi(input_handle_, audio_data.data(), frames);
    }
    
    if (result < 0) {
        std::cerr << "[AlsaHandler] 读取音频数据失败: " << snd_strerror(result) << std::endl;
        return false;
    }
    
    audio_data.resize(result * channels_);
    return true;
}

bool AlsaHandler::writeAudioData(const AudioData& audio_data) {
    if (!output_handle_ || audio_data.empty()) {
        return false;
    }
    
    snd_pcm_sframes_t result;
    size_t frames = audio_data.size() / channels_;
    size_t offset = 0;
    
    while (offset < frames) {
        result = snd_pcm_writei(output_handle_, audio_data.data() + offset * channels_, 
                               frames - offset);
        
        if (result == -EPIPE) {
            // 缓冲区欠载，恢复
            snd_pcm_recover(output_handle_, result, 0);
            result = snd_pcm_writei(output_handle_, audio_data.data() + offset * channels_, 
                                   frames - offset);
        } else if (result < 0) {
            std::cerr << "[AlsaHandler] 写入音频数据失败: " << snd_strerror(result) << std::endl;
            return false;
        }
        
        offset += result;
    }
    
    return true;
}

void AlsaHandler::closeAudioDevices() {
    if (input_handle_) {
        snd_pcm_drain(input_handle_);
        snd_pcm_close(input_handle_);
        input_handle_ = nullptr;
    }
    
    if (output_handle_) {
        snd_pcm_drain(output_handle_);
        snd_pcm_close(output_handle_);
        output_handle_ = nullptr;
    }
}

void AlsaHandler::startCapture() {
    if (input_handle_) {
        snd_pcm_start(input_handle_);
    }
}

void AlsaHandler::stopCapture() {
    if (input_handle_) {
        snd_pcm_drop(input_handle_);
    }
}

void AlsaHandler::startPlayback() {
    if (output_handle_) {
        snd_pcm_start(output_handle_);
    }
}

void AlsaHandler::stopPlayback() {
    if (output_handle_) {
        snd_pcm_drop(output_handle_);
    }
}

} // namespace xiaozhi
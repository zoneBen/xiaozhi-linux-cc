#pragma once

#include <string>
#include <alsa/asoundlib.h>
#include "xiaozhi_types.h"

namespace xiaozhi {

class AlsaHandler {
public:
    AlsaHandler();
    ~AlsaHandler();

    bool initialize(int sample_rate = 16000, int channels = 1, const std::string& device_name = "");
    
    // 音频数据读取/写入
    bool readAudioData(AudioData& audio_data, size_t frames = 320);  // 默认60ms数据 (16kHz下)
    bool writeAudioData(const AudioData& audio_data);

    // 控制方法
    void startCapture();
    void stopCapture();
    void startPlayback();
    void stopPlayback();

private:
    bool openAudioDevices();
    bool configureAudioParams(snd_pcm_t* handle, bool is_capture);
    void closeAudioDevices();

    snd_pcm_t* input_handle_;
    snd_pcm_t* output_handle_;
    
    int sample_rate_;
    int channels_;
    std::string device_name_;
};

} // namespace xiaozhi
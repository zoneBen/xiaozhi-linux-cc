#pragma once

#include <thread>
#include <mutex>
#include <atomic>
#include <functional>
#include "xiaozhi_types.h"

namespace xiaozhi {

class AudioManager {
public:
    AudioManager();
    ~AudioManager();

    bool initialize(int sample_rate = 16000, int channels = 1);
    void startRecording();
    void stopRecording();
    void startPlayback();
    void stopPlayback();

    void process();

    // 设置音频数据回调
    void setRecordCallback(std::function<void(const AudioData&)> callback);
    void setPlaybackCallback(std::function<void(AudioData&)> callback);

private:
    std::atomic<bool> initialized_{false};
    std::atomic<bool> recording_{false};
    std::atomic<bool> playing_{false};

    int sample_rate_;
    int channels_;

    std::function<void(const AudioData&)> record_callback_;
    std::function<void(AudioData&)> playback_callback_;

    std::thread record_thread_;
    std::thread play_thread_;
    std::mutex mutex_;

    // 内部音频处理函数
    void recordLoop();
    void playLoop();
};

} // namespace xiaozhi
#pragma once

#include <vector>
#include <cstdint>
#include "xiaozhi_types.h"

typedef struct OpusEncoder OpusEncoderStruct;

namespace xiaozhi {

class OpusEncoder {
public:
    OpusEncoder();
    ~OpusEncoder();

    bool initialize(int sample_rate = 16000, int channels = 1, int bitrate = 32000);
    
    // 编码PCM数据为Opus格式
    std::vector<uint8_t> encode(const AudioData& pcm_data);

private:
    void cleanup();

    OpusEncoderStruct* encoder_;
    bool initialized_;
    
    int sample_rate_;
    int channels_;
    int bitrate_;
};

} // namespace xiaozhi
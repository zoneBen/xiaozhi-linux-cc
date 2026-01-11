#pragma once

#include <vector>
#include <cstdint>
#include "xiaozhi_types.h"

typedef struct OpusDecoder OpusDecoderStruct;

namespace xiaozhi {

class OpusDecoder {
public:
    OpusDecoder();
    ~OpusDecoder();

    bool initialize(int sample_rate = 16000, int channels = 1);
    
    // 解码Opus数据为PCM格式
    AudioData decode(const std::vector<uint8_t>& opus_data);

private:
    void cleanup();

    OpusDecoderStruct* decoder_;
    bool initialized_;
    
    int sample_rate_;
    int channels_;
};

} // namespace xiaozhi
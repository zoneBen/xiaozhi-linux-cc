#!/bin/bash

# 小智AI音频设备测试脚本
# 用于测试Rockchip设备或其他Linux系统的音频功能

echo "小智AI音频设备测试脚本"
echo "========================="

# 检查音频设备
echo "1. 检查音频设备..."
echo "录音设备:"
arecord -l
echo ""
echo "播放设备:"
aplay -l
echo ""

# 创建测试目录
TEST_DIR="/tmp/xiaozhi_audio_test"
mkdir -p "$TEST_DIR"

# 测试录音
echo "2. 测试录音功能 (3秒)..."
RECORD_FILE="$TEST_DIR/test_record.wav"
echo "开始录音3秒，请说话..."
arecord -D hw:0,0 -f S16_LE -r 16000 -c 1 -d 3 "$RECORD_FILE" 2>/dev/null
if [ $? -eq 0 ]; then
    echo "✓ 录音成功: $RECORD_FILE"
else
    echo "✗ 录音失败，尝试使用默认设备..."
    arecord -D default -f S16_LE -r 16000 -c 1 -d 3 "$RECORD_FILE" 2>/dev/null
    if [ $? -eq 0 ]; then
        echo "✓ 录音成功: $RECORD_FILE"
    else
        echo "✗ 录音失败"
        RECORD_FILE=""
    fi
fi

# 测试播放
if [ -n "$RECORD_FILE" ] && [ -f "$RECORD_FILE" ]; then
    echo "3. 测试播放功能..."
    echo "播放录制的音频..."
    aplay -D hw:0,0 "$RECORD_FILE" 2>/dev/null
    if [ $? -eq 0 ]; then
        echo "✓ 播放成功"
    else
        echo "✗ 播放失败，尝试使用默认设备..."
        aplay -D default "$RECORD_FILE" 2>/dev/null
        if [ $? -eq 0 ]; then
            echo "✓ 播放成功"
        else
            echo "✗ 播放失败"
        fi
    fi
fi

# 测试其他可能的设备
echo "4. 测试其他可能的音频设备..."
CARDS=$(arecord -l | grep "card [0-9]" | cut -d ' ' -f 2 | tr -d ':')
for card in $CARDS; do
    echo "尝试设备 hw:$card,0..."
    TEST_FILE="$TEST_DIR/test_$card.wav"
    arecord -D hw:$card,0 -f S16_LE -r 16000 -c 1 -d 1 "$TEST_FILE" 2>/dev/null
    if [ $? -eq 0 ]; then
        echo "✓ 可以使用设备 hw:$card,0"
        rm -f "$TEST_FILE"
    else
        echo "✗ 设备 hw:$card,0 不可用"
    fi
done

echo ""
echo "5. 生成推荐配置..."
echo "{"
echo "  \"audio\": {"
echo "    \"input_device\": \"hw:0,0\","  
echo "    \"output_device\": \"hw:0,0\","
echo "    \"sample_rate\": 16000,"
echo "    \"channels\": 1,"
echo "    \"format\": \"S16_LE\""
echo "  }"
echo "}"

echo ""
echo "测试完成！"
echo "请根据测试结果选择合适的音频设备名称，并更新配置文件。"
echo "推荐使用测试成功的设备名称，如 hw:0,0 或其他测试通过的设备。"

# 清理
#rm -rf "$TEST_DIR"
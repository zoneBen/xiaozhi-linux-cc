# 小智AI - Linux版

小智AI是一款基于先进AI技术的智能语音聊天机器人系统。本项目是针对Linux系统实现的小智AI功能版本，提供了完整的AI交互、音频处理和网络通信能力。

## 功能特性

- 🗣️ **智能语音交互** - 支持语音输入输出，与AI模型进行自然对话
- 🔌 **多种通信协议** - 支持WebSocket、MQTT等多种通信方式
- 🎧 **高质量音频处理** - 使用Opus编解码器，支持实时音频流传输
- 🌐 **MCP协议支持** - Model Context Protocol，用于物联网设备控制
- 📱 **跨平台兼容** - 专为Linux系统优化设计
- ⚡ **实时响应** - 低延迟音频处理和AI交互

## 系统架构

```
┌─────────────────┐    ┌──────────────────┐    ┌─────────────────┐
│                 │    │                  │    │                 │
│   AI服务器      │◄──►│   MCP协议层      │◄──►│   Linux客户端   │
│                 │    │                  │    │                 │
└─────────────────┘    └──────────────────┘    └─────────────────┘
                                │
                       ┌──────────────────┐
                       │  通信协议层      │
                       │  (WebSocket/MQTT)│
                       └──────────────────┘
                                │
                       ┌──────────────────┐
                       │  音频数据传输    │
                       │  (Audio Stream)  │
                       └──────────────────┘
```

## 核心模块

### 1. 音频处理模块
- **音频输入** - 通过ALSA/PulseAudio捕获麦克风输入
- **音频输出** - 支持扬声器播放AI回复
- **音频编解码** - 使用Opus编解码器进行高效音频压缩
- **实时处理** - 低延迟音频流处理

### 2. 网络通信模块
- **WebSocket协议** - 主要通信通道，支持控制消息和音频数据传输
- **MQTT协议** - 支持消息队列遥测传输协议
- **连接管理** - 自动重连、状态监控、错误处理

### 3. MCP协议模块
- **工具发现** - 自动发现设备支持的功能
- **远程控制** - 通过JSON-RPC 2.0协议控制设备
- **状态同步** - 实时同步设备状态信息

### 4. AI交互模块
- **语音识别** - 将语音转换为文本
- **对话管理** - 管理对话状态和上下文
- **语音合成** - 将AI回复转换为语音输出

### 5. 用户界面模块
- **命令行界面** - 提供直观的CLI操作
- **状态显示** - 实时显示连接状态和交互信息
- **配置管理** - 灵活的配置选项

## 安装要求

### 系统要求
- Linux发行版 (Ubuntu 18.04+, Debian 10+, CentOS 8+)
- x86_64 或 ARM64 架构
- 至少 2GB RAM
- 至少 500MB 磁盘空间

### 依赖库
- ALSA开发库 (`alsa-lib-dev`)
- OpenSSL (`libssl-dev`)
- JSON-C库 (`libjson-c-dev`)
- libopus (`libopus-dev`)
- libopusenc (`libopusenc-dev`)
- libopusfile (`libopusfile-dev`)

### 运行时依赖
- PulseAudio 或 ALSA音频系统
- 网络连接
- 麦克风和扬声器设备

## 安装步骤

### 1. 克隆项目
```bash
git clone https://github.com/your-repo/xiaozhi-linux-cc.git
cd xiaozhi-linux-cc
```

### 2. 安装依赖
```bash
# Ubuntu/Debian
sudo apt update
sudo apt install build-essential alsa-utils libasound2-dev libssl-dev \
               libjson-c-dev libopus-dev libopusenc-dev libopusfile-dev \
               cmake pkg-config

# CentOS/RHEL/Fedora
sudo yum install gcc gcc-c++ make alsa-lib-devel openssl-devel \
               json-c-devel opus-devel opus-tools
```

### 3. 构建项目
```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### 4. 配置音频权限
```bash
# 添加用户到audio组
sudo usermod -a -G audio $USER
# 重新登录以应用更改
```

## 配置说明

### 主配置文件
配置文件位于 `~/.config/xiaozhi/config.json`，示例配置：

```json
{
  "server": {
    "websocket_url": "wss://api.xiaozhi.example.com/ws",
    "mqtt_broker": "mqtt://broker.xiaozhi.example.com:1883",
    "auth_token": "your_auth_token_here"
  },
  "audio": {
    "input_device": "default",
    "output_device": "default",
    "sample_rate": 16000,
    "channels": 1,
    "opus_bitrate": 32000
  },
  "mcp": {
    "enabled": true,
    "port": 8080
  },
  "network": {
    "reconnect_interval": 5,
    "timeout": 30
  }
}
```

### 音频设备配置
- 输入设备：通常为 `default` 或 `hw:0,0`
- 输出设备：通常为 `default` 或 `hw:0,0`
- 采样率：支持 8000, 16000, 24000, 48000 Hz

## 使用方法

### 1. 启动服务
```bash
# 前台运行
./xiaozhi-daemon --config ~/.config/xiaozhi/config.json

# 后台运行
nohup ./xiaozhi-daemon --config ~/.config/xiaozhi/config.json > xiaozhi.log 2>&1 &
```

### 2. 命令行交互
```bash
# 测试音频设备
./xiaozhi-cli --test-audio

# 语音对话模式
./xiaozhi-cli --voice-mode

# 文本对话模式
./xiaozhi-cli --text-mode
```

### 3. 系统服务
```bash
# 安装系统服务
sudo cp xiaozhi.service /etc/systemd/system/
sudo systemctl daemon-reload
sudo systemctl enable xiaozhi
sudo systemctl start xiaozhi
```

## API接口

### WebSocket接口
- **连接URL**: `wss://api.xiaozhi.example.com/ws`
- **认证**: HTTP头部 `Authorization: Bearer <token>`
- **消息格式**: JSON格式，支持以下类型：
  - `hello`: 握手消息
  - `listen`: 开始/停止监听
  - `stt`: 语音识别结果
  - `tts`: 文本转语音控制
  - `mcp`: 设备控制消息

### MCP协议接口
- **方法**: `initialize`, `tools/list`, `tools/call`
- **格式**: JSON-RPC 2.0
- **传输**: 通过WebSocket或HTTP

## 环境变量

- `XIAOZHI_CONFIG_PATH`: 配置文件路径
- `XIAOZHI_LOG_LEVEL`: 日志级别 (DEBUG, INFO, WARN, ERROR)
- `XIAOZHI_AUDIO_INPUT`: 音频输入设备
- `XIAOZHI_AUDIO_OUTPUT`: 音频输出设备

## 日志管理

日志文件默认存储在 `~/.local/share/xiaozhi/logs/` 目录下：
- `xiaozhi.log`: 主日志文件
- `audio.log`: 音频处理日志
- `network.log`: 网络通信日志

## 故障排除

### 常见问题

1. **音频设备无法访问**
   ```bash
   # 检查音频设备
   arecord -l
   aplay -l
   
   # 检查用户权限
   groups $USER
   ```

2. **连接服务器失败**
   ```bash
   # 检查网络连接
   ping api.xiaozhi.example.com
   
   # 检查防火墙设置
   sudo ufw status
   ```

3. **权限不足**
   ```bash
   # 检查配置文件权限
   ls -la ~/.config/xiaozhi/
   
   # 修复权限
   chmod 600 ~/.config/xiaozhi/config.json
   ```

### 调试模式
```bash
# 启用详细日志
export XIAOZHI_LOG_LEVEL=DEBUG
./xiaozhi-daemon --verbose
```

## 性能优化

### 音频性能
- 使用专用音频接口减少延迟
- 调整缓冲区大小以平衡延迟和稳定性
- 确保音频线程优先级设置正确

### 网络性能
- 选择地理位置最近的服务器
- 使用稳定的网络连接
- 配置适当的超时和重连参数

## 安全考虑

- 使用TLS加密所有网络通信
- 定期更新认证令牌
- 限制配置文件的文件权限
- 定期审查日志中的异常活动

## 开发贡献

### 代码结构
```
xiaozhi-linux-cc/
├── src/                    # 源代码
│   ├── audio/             # 音频处理模块
│   ├── network/           # 网络通信模块  
│   ├── mcp/               # MCP协议模块
│   ├── ai/                # AI交互模块
│   └── cli/               # 命令行界面
├── include/               # 头文件
├── configs/               # 配置文件模板
├── scripts/               # 构建和部署脚本
└── docs/                  # 文档
```

### 构建系统
- 使用CMake进行跨平台构建
- 支持静态和动态链接
- 包含单元测试和集成测试

### 贡献指南
1. Fork项目
2. 创建功能分支
3. 提交更改
4. 发起Pull Request

## 许可证

本项目采用 [MIT许可证](LICENSE)，详情请参阅 LICENSE 文件。

## 支持与反馈

- **问题报告**: [GitHub Issues](https://github.com/your-repo/xiaozhi-linux-cc/issues)
- **文档**: [docs/](docs/) 目录包含详细的技术文档
- **社区**: 加入我们的开发者社区获取支持

---

感谢您使用小智AI Linux版！如果您有任何问题或建议，请随时与我们联系。
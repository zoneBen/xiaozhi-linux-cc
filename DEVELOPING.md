# 小智AI Linux版 - 开发指南

## 项目结构

```
xiaozhi-linux-cc/
├── src/                    # 源代码
│   ├── audio/             # 音频处理模块
│   ├── network/           # 网络通信模块  
│   ├── mcp/               # MCP协议模块
│   ├── ai/                # AI交互模块
│   ├── cli/               # 命令行界面
│   └── utils/             # 工具模块
├── include/               # 公共头文件
├── configs/               # 配置文件
├── scripts/               # 构建和部署脚本
├── docs/                  # 文档
├── tests/                 # 测试文件
├── build/                 # 构建输出目录
├── assets/                # 资源文件
├── systemd/               # 系统服务配置
├── CMakeLists.txt         # 构建配置
├── README.md              # 项目说明
└── LICENSE                # 许可证
```

## 模块说明

### 音频模块 (src/audio/)
负责音频输入输出处理，包括：
- 音频设备管理
- Opus编解码
- 音频流处理

### 网络模块 (src/network/)
处理网络通信，包括：
- WebSocket客户端
- MQTT客户端
- 协议处理

### MCP模块 (src/mcp/)
实现MCP协议，包括：
- 工具注册
- JSON-RPC处理
- 设备控制

### AI模块 (src/ai/)
处理AI交互逻辑，包括：
- 对话管理
- API接口
- 响应处理

## 开发规范

- 使用C++17标准
- 遵循Google C++代码风格
- 所有公共API需要文档注释
- 单元测试覆盖率不低于80%

## 调试和测试

### 调试模式

```bash
# 启用调试日志
export XIAOZHI_LOG_LEVEL=DEBUG
./build/xiaozhi-daemon --config ~/.config/xiaozhi/config.json
```

### 音频测试

```bash
# 测试音频设备
./build/xiaozhi-cli --test-audio

# 录制音频测试
arecord -D default -f cd -d 5 test.wav

# 播放音频测试
aplay test.wav
```

### 网络测试

```bash
# 测试WebSocket连接
./build/xiaozhi-cli --text-mode
```

## 添加新功能

### 添加新的音频编解码器

1. 在 `src/audio/` 目录下创建新的编解码器类
2. 实现编解码接口
3. 在 `AudioManager` 中集成新编解码器

### 添加新的通信协议

1. 在 `src/network/` 目录下创建新的协议实现
2. 实现协议接口
3. 在主程序中集成新协议

### 添加新的MCP工具

1. 使用 `McpServer::addTool` 方法注册新工具
2. 定义工具参数和处理函数
3. 实现工具功能逻辑

## 贡献指南

1. Fork项目
2. 创建功能分支 (`git checkout -b feature/amazing-feature`)
3. 提交更改 (`git commit -m 'Add some amazing feature'`)
4. 推送到分支 (`git push origin feature/amazing-feature`)
5. 发起Pull Request

## 构建和发布

### 构建不同类型的版本

```bash
# Debug版本（包含调试信息）
./scripts/build.sh --type Debug

# Release版本（优化性能）
./scripts/build.sh --type Release

# 清理后重新构建
./scripts/build.sh --clean --type RelWithDebInfo
```

### 自定义安装路径

```bash
# 安装到自定义路径
./scripts/build.sh --install-prefix /opt/xiaozhi
```

## 故障排除

### 常见编译错误

1. **找不到依赖库**
   - 确保已安装所有必需的开发包
   - 检查 pkg-config 是否能找到库文件

2. **权限错误**
   - 确保用户属于 audio 组
   - 检查配置文件权限

3. **音频设备错误**
   - 检查音频设备是否被其他程序占用
   - 验证 ALSA/PulseAudio 配置

### 调试技巧

1. 使用日志输出调试信息
2. 启用详细日志级别
3. 分模块独立测试功能
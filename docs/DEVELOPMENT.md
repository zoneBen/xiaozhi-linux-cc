# 小智AI Linux版 - 开发文档

## 项目结构

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

## 编译构建

\\\ash
mkdir build && cd build
cmake ..
make -j
\\\

## 开发规范

- 使用C++17标准
- 遵循Google C++代码风格
- 所有公共API需要文档注释
- 单元测试覆盖率不低于80%

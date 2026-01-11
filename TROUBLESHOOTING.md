# 小智AI Linux版 - 故障排除指南

本文档提供常见问题的解决方案，特别是构建和运行时遇到的问题。

## 常见构建问题

### 1. 依赖库缺失

#### 问题：`E: Unable to locate package libopusenc-dev`

**原因**: 在某些Ubuntu/Debian版本中，`libopusenc-dev`包可能位于不同的软件源中。

**解决方案**:

1. **更新包列表并尝试安装**:
   ```bash
   sudo apt update
   sudo apt install libopusenc-dev
   ```

2. **安装opus相关包**:
   ```bash
   sudo apt install opus-tools libopus-dev libopusenc-dev libopusfile-dev
   ```

3. **启用universe仓库（Ubuntu）**:
   ```bash
   sudo add-apt-repository universe
   sudo apt update
   sudo apt install libopusenc-dev
   ```

4. **从源码编译安装（适用于所有Linux发行版）**:
   ```bash
   # 安装基础依赖
   sudo apt install build-essential autoconf automake libtool pkg-config libopus-dev
   
   # 克隆源码
   git clone https://github.com/xiph/libopusenc.git
   cd libopusenc
   
   # 配置、编译和安装
   ./autogen.sh
   ./configure
   make
   sudo make install
   
   # 更新库缓存
   sudo ldconfig
   ```

#### 问题：`pkg-config --exists opusenc` 检查失败

**原因**: opusenc库已安装，但pkg-config找不到它。

**解决方案**:

1. **检查库是否安装**:
   ```bash
   ldconfig -p | grep opus
   ```

2. **检查pkg-config路径**:
   ```bash
   pkg-config --cflags --libs opus
   pkg-config --cflags --libs opusenc
   ```

3. **手动指定PKG_CONFIG_PATH**:
   ```bash
   export PKG_CONFIG_PATH="/usr/local/lib/pkgconfig:$PKG_CONFIG_PATH"
   ```

4. **验证库文件是否正确安装**:
   ```bash
   # 检查库文件是否存在
   ls -la /usr/local/lib/libopusenc*
   ls -la /usr/local/include/opusenc*
   
   # 检查pkg-config文件
   ls -la /usr/local/lib/pkgconfig/opusenc.pc
   
   # 验证pkg-config能否找到库
   PKG_CONFIG_PATH="/usr/local/lib/pkgconfig:$PKG_CONFIG_PATH" pkg-config --exists opusenc && echo "opusenc found" || echo "opusenc not found"
   pkg-config --modversion opusenc
   
   # 如果库存在但pkg-config找不到，临时设置环境变量
   export PKG_CONFIG_PATH="/usr/local/lib/pkgconfig:$PKG_CONFIG_PATH"
   ```

5. **修复脚本换行符问题（如果遇到`#!/bin/bash: No such file or directory`错误）**:
   ```bash
   # 修复Windows创建的脚本在Linux上的换行符问题
   sed -i 's/\r$//' scripts/*.sh
   
   # 或者使用dos2unix（如果已安装）
   dos2unix scripts/*.sh
   ```

6. **修复pkg-config文件缺失问题（当库文件存在但pkg-config找不到时）**:
   ```bash
   # 首先检查是否有.pc文件
   find /usr/local -name "opusenc.pc" 2>/dev/null
   
   # 如果没有找到.pc文件，手动创建它
   sudo mkdir -p /usr/local/lib/pkgconfig
   sudo tee /usr/local/lib/pkgconfig/opusenc.pc > /dev/null <<EOF
prefix=/usr/local
exec_prefix=${prefix}
libdir=${exec_prefix}/lib
includedir=${prefix}/include

Name: opusenc
Description: Opus Audio Encoder Library
Version: 0.2.1
Libs: -L${libdir} -lopusenc
Cflags: -I${includedir}/opus
Requires: opus
EOF
   
   # 验证pkg-config现在是否能找到库
   export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig:$PKG_CONFIG_PATH
   pkg-config --exists opusenc && echo "Found" || echo "Not found"
   ```

7. **修复头文件缺失问题（如 opus.h: No such file or directory）**:
   ```bash
   # 问题：opusenc.h 依赖于 opus.h，但编译器找不到 opus.h
   # 这通常是因为 libopus-dev 没有安装，或者安装路径不在编译器搜索路径中
   
   # 检查头文件位置
   find /usr -name "opus.h" 2>/dev/null
   find /usr -name "opusenc.h" 2>/dev/null
   
   # 如果 opus.h 在 /usr/include/opus/opus.h
   # 而 opusenc.h 在 /usr/local/include/opus/opusenc.h
   # 则需要确保编译器能找到两个路径
   
   # 解决方案1：安装 opus 开发包
   sudo apt install libopus-dev
   
   # 解决方案2：如果从源码安装，确保安装了头文件并更新路径
   # 从源码安装 opus 库：
   git clone https://github.com/xiph/opus.git
   cd opus
   ./autogen.sh
   ./configure
   make
   sudo make install
   sudo ldconfig  # 重要：更新库缓存
   
   # 验证头文件是否存在
   find /usr -name "opus.h" 2>/dev/null
   find /usr -name "opusenc.h" 2>/dev/null
   ```

### 2. 音频权限问题

#### 问题：无法访问音频设备

**解决方案**:

1. **检查用户是否属于audio组**:
   ```bash
   groups $USER
   ```

2. **添加用户到audio组**:
   ```bash
   sudo usermod -a -G audio $USER
   # 重新登录使更改生效
   ```

3. **检查音频设备**:
   ```bash
   arecord -l
   aplay -l
   ```

### 3. CMake配置问题

#### 问题：找不到依赖库路径

**解决方案**:

1. **指定CMAKE_PREFIX_PATH**:
   ```bash
   cmake .. -DCMAKE_PREFIX_PATH=/usr/local
   ```

2. **检查库路径**:
   ```bash
   ldconfig -p | grep -i asound
   ldconfig -p | grep -i json
   ```

## 运行时问题

### 1. 程序崩溃或异常退出

**排查步骤**:

1. **启用调试日志**:
   ```bash
   export XIAOZHI_LOG_LEVEL=DEBUG
   ./xiaozhi-daemon --config ~/.config/xiaozhi/config.json
   ```

2. **检查配置文件**:
   ```bash
   cat ~/.config/xiaozhi/config.json
   ```

### 2. 网络连接失败

**排查步骤**:

1. **检查网络连接**:
   ```bash
   ping -c 3 api.xiaozhi.example.com
   ```

2. **检查防火墙设置**:
   ```bash
   sudo ufw status
   ```

## 调试工具

### 1. 使用依赖检查脚本

项目包含一个依赖检查脚本，可自动检测系统并提供建议：

```bash
./scripts/check_deps.sh
```

### 2. 使用测试构建脚本

测试构建脚本提供更详细的错误信息：

```bash
./scripts/test_build.sh
```

## 系统要求验证

### 验证构建工具

```bash
which gcc g++ make cmake pkg-config
gcc --version
cmake --version
```

### 验证库文件

```bash
pkg-config --exists alsa openssl json-c opus opusenc opusfile && echo "All libraries found" || echo "Some libraries missing"
```

## 其他资源

- [INSTALL.md](INSTALL.md) - 详细安装说明
- [DEVELOPING.md](DEVELOPING.md) - 开发指南
- [README.MD](README.MD) - 项目介绍和基本使用

如遇到以上未涵盖的问题，请检查项目文档或提交Issue。
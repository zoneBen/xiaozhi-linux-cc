# 小智AI Linux版 - 安装说明

## 系统要求

- Linux发行版 (Ubuntu 18.04+, Debian 10+, CentOS 8+)
- x86_64 或 ARM64 架构
- 至少 2GB RAM
- 至少 500MB 磁盘空间

## 依赖安装

### Ubuntu/Debian

```bash
sudo apt update
sudo apt install build-essential alsa-utils libasound2-dev libssl-dev \
               libjson-c-dev libopus-dev libopusenc-dev libopusfile-dev \
               cmake pkg-config
```

**注意**: 如果遇到 `E: Unable to locate package libopusenc-dev` 错误，请尝试以下命令之一：

```bash
# 选项1：更新包索引并安装
sudo apt update && sudo apt install libopusenc-dev

# 选项2：安装opus工具包
sudo apt install opus-tools libopus-dev libopusenc-dev libopusfile-dev

# 选项3：启用universe仓库（Ubuntu）
sudo add-apt-repository universe
sudo apt update
sudo apt install libopusenc-dev

# 选项4：从源码编译安装（适用于所有Linux发行版）
sudo apt install build-essential autoconf automake libtool pkg-config libopus-dev
git clone https://github.com/xiph/libopusenc.git
cd libopusenc
./autogen.sh
./configure
make
sudo make install
sudo ldconfig  # 更新库缓存

# 如果构建时仍然提示找不到opusenc库，请设置PKG_CONFIG_PATH：
export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig:$PKG_CONFIG_PATH
# 或者添加到~/.bashrc中以永久生效：
echo 'export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig:$PKG_CONFIG_PATH' >> ~/.bashrc
source ~/.bashrc

# 如果仍然找不到库，请检查是否有.pc文件：
find /usr/local -name "opusenc.pc" 2>/dev/null

# 注意：如果从源码编译安装，需要确保同时安装了 opus 和 opusenc 的头文件
# opusenc.h 依赖于 opus.h，两者都需要在编译时可访问
# 确保执行了：sudo ldconfig 更新库缓存

# 如果没有.pc文件，手动创建它：
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
```

### CentOS/RHEL/Fedora

```bash
sudo yum install gcc gcc-c++ make alsa-lib-devel openssl-devel \
               json-c-devel opus-devel opus-tools cmake pkg-config
```

## 音频权限配置

为了访问音频设备，需要将用户添加到audio组：

```bash
sudo usermod -a -G audio $USER
# 重新登录以应用更改
```

### Rockchip设备特殊配置

对于使用Rockchip音频芯片（如ES8388）的设备，您可能需要：

```bash
# 检查音频设备
arecord -l  # 列出录音设备
aplay -l    # 列出播放设备

# 针对Rockchip设备的典型配置
{
  "audio": {
    "input_device": "hw:0,0",  # 使用硬件设备，而不是默认设备
    "output_device": "hw:0,0",
    "sample_rate": 16000,
    "channels": 1,
    "format": "S16_LE"
  }
}

# 测试音频

speaker-test -c 2 -t wav -D hw:0,0

```

## 构建项目

```bash
# 克隆项目
git clone https://github.com/your-repo/xiaozhi-linux-cc.git
cd xiaozhi-linux-cc

# 构建项目
./scripts/build.sh

# 或者指定构建类型
./scripts/build.sh --type Debug
```

## 安装和运行

### 安装到系统

```bash
# 安装到默认位置 (/usr/local)
sudo make install

# 或者指定安装路径
sudo ./scripts/build.sh --install-prefix /opt/xiaozhi
```

### 配置文件

首次运行前需要创建配置文件：

```bash
# 创建配置目录
mkdir -p ~/.config/xiaozhi

# 复制默认配置
cp configs/default_config.json ~/.config/xiaozhi/config.json

# 编辑配置文件，填入实际的服务器地址和认证信息
nano ~/.config/xiaozhi/config.json
```

### 运行方式

#### 1. 直接运行

```bash
# 前台运行
./build/xiaozhi-daemon --config ~/.config/xiaozhi/config.json

# 后台运行
nohup ./build/xiaozhi-daemon --config ~/.config/xiaozhi/config.json > xiaozhi.log 2>&1 &
```

#### 2. 作为系统服务运行

```bash
# 安装系统服务
sudo cp systemd/xiaozhi.service /etc/systemd/system/
sudo systemctl daemon-reload
sudo systemctl enable xiaozhi
sudo systemctl start xiaozhi

# 查看服务状态
sudo systemctl status xiaozhi

# 查看日志
sudo journalctl -u xiaozhi -f
```

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

2. **依赖库缺失**

   根据构建脚本的提示安装相应的开发包。

3. **权限不足**

   确保用户属于audio组，并具有访问配置文件的权限。

### 调试模式

```bash
# 启用详细日志
export XIAOZHI_LOG_LEVEL=DEBUG
./build/xiaozhi-daemon --verbose
```

## 环境变量

- `XIAOZHI_CONFIG_PATH`: 配置文件路径
- `XIAOZHI_LOG_LEVEL`: 日志级别 (DEBUG, INFO, WARN, ERROR)
- `XIAOZHI_AUDIO_INPUT`: 音频输入设备
- `XIAOZHI_AUDIO_OUTPUT`: 音频输出设备
#!/bin/bash

# 小智AI Linux版测试构建脚本

set -e  # 遇到错误时退出

echo "小智AI Linux版测试构建脚本"
echo

# 检查是否在Linux环境下
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    echo "检测到Linux环境"
else
    echo "警告: 当前不在Linux环境下，某些功能可能无法正常工作"
fi

# 检查必需的命令
echo "检查构建依赖..."

# 检查基本命令
BASIC_CMDS=("gcc" "g++" "make" "cmake" "pkg-config" "git")
for cmd in "${BASIC_CMDS[@]}"; do
    if ! command -v "$cmd" &> /dev/null; then
        echo "错误: 未找到命令 $cmd"
        exit 1
    fi
done

echo "基本命令检查通过"

# 检查必要的库
LIBS_TO_CHECK=("alsa" "openssl" "json-c" "opus" "opusenc" "opusfile")

echo "检查必要的库..."
MISSING_LIBS=()

for lib in "${LIBS_TO_CHECK[@]}"; do
    if pkg-config --exists "$lib"; then
        echo "  ✓ $lib"
    else
        echo "  ✗ $lib"
        MISSING_LIBS+=("$lib")
    fi
done

if [ ${#MISSING_LIBS[@]} -ne 0 ]; then
    echo
    echo "错误: 未找到以下库: ${MISSING_LIBS[*]}"
    echo
    echo "Ubuntu/Debian 安装命令:"
    echo "  sudo apt update && sudo apt install build-essential alsa-utils libasound2-dev libssl-dev libjson-c-dev libopus-dev libopusenc-dev libopusfile-dev cmake pkg-config"
    echo
    echo "CentOS/RHEL/Fedora 安装命令:"
    echo "  sudo yum install gcc gcc-c++ make alsa-lib-devel openssl-devel json-c-devel opus-devel opus-tools cmake pkg-config"
    echo
    exit 1
fi

echo
echo "所有依赖检查通过！"

# 创建构建目录
BUILD_DIR="build-test"
if [ -d "$BUILD_DIR" ]; then
    echo "清理旧的测试构建目录..."
    rm -rf "$BUILD_DIR"
fi

echo "创建测试构建目录..."
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# 配置CMake项目
echo "配置CMake项目..."
cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

echo

# 编译项目
echo "开始编译项目..."
NPROC=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
make -j"$NPROC"

echo
echo "编译完成！"

# 检查生成的可执行文件
if [ -f "xiaozhi-daemon" ]; then
    echo "✓ 成功生成 xiaozhi-daemon 可执行文件"
    ls -la xiaozhi-daemon
else
    echo "✗ 未找到 xiaozhi-daemon 可执行文件"
    ls -la
fi

echo
echo "测试构建完成！"
echo "要运行程序，执行: ./xiaozhi-daemon --config ../configs/default_config.json"
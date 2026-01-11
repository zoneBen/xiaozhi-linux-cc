#!/bin/bash

# 小智AI Linux版构建脚本

set -e  # 遇到错误时退出

# 默认参数
BUILD_TYPE="Release"
INSTALL_PREFIX="/usr/local"
CLEAN_BUILD=false

# 解析命令行参数
while [[ $# -gt 0 ]]; do
    case $1 in
        -t|--type)
            BUILD_TYPE="$2"
            shift 2
            ;;
        -i|--install-prefix)
            INSTALL_PREFIX="$2"
            shift 2
            ;;
        -c|--clean)
            CLEAN_BUILD=true
            shift
            ;;
        -h|--help)
            echo "用法: $0 [选项]"
            echo "选项:"
            echo "  -t, --type TYPE          构建类型 (Debug|Release|RelWithDebInfo|MinSizeRel) [默认: Release]"
            echo "  -i, --install-prefix DIR 安装路径前缀 [默认: /usr/local]"
            echo "  -c, --clean            清理之前的构建文件"
            echo "  -h, --help             显示此帮助信息"
            exit 0
            ;;
        *)
            echo "未知选项: $1"
            exit 1
            ;;
    esac
done

echo "小智AI Linux版构建脚本"
echo "构建类型: $BUILD_TYPE"
echo "安装路径: $INSTALL_PREFIX"
echo

# 检查依赖
echo "检查构建依赖..."
REQUIRED_CMDS=("cmake" "make" "g++" "pkg-config")
for cmd in "${REQUIRED_CMDS[@]}"; do
    if ! command -v "$cmd" &> /dev/null; then
        echo "错误: 未找到命令 $cmd"
        exit 1
    fi
done

# 检查必要的库
REQUIRED_LIBS=("alsa" "openssl" "json-c" "opus" "opusenc" "opusfile")
MISSING_LIBS=()
for lib in "${REQUIRED_LIBS[@]}"; do
    if ! pkg-config --exists "$lib"; then
        MISSING_LIBS+=("$lib")
    fi
done

if [ ${#MISSING_LIBS[@]} -ne 0 ]; then
    echo "错误: 未找到以下库: ${MISSING_LIBS[*]}"
    echo "请安装缺失的依赖包:"
    echo "Ubuntu/Debian: sudo apt update && sudo apt install build-essential alsa-utils libasound2-dev libssl-dev libjson-c-dev libopus-dev libopusenc-dev libopusfile-dev cmake pkg-config"
    echo "CentOS/RHEL/Fedora: sudo yum install gcc gcc-c++ make alsa-lib-devel openssl-devel json-c-devel opus-devel opus-tools"
    exit 1
fi

echo "所有依赖检查通过"
echo

# 创建构建目录
BUILD_DIR="build"
if [ "$CLEAN_BUILD" = true ]; then
    echo "清理构建目录..."
    rm -rf "$BUILD_DIR"
fi

if [ ! -d "$BUILD_DIR" ]; then
    echo "创建构建目录..."
    mkdir -p "$BUILD_DIR"
fi

cd "$BUILD_DIR"

# 配置CMake项目
echo "配置CMake项目..."
cmake .. \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    -DCMAKE_INSTALL_PREFIX="$INSTALL_PREFIX" \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

echo

# 编译项目
echo "开始编译项目..."
NPROC=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
make -j"$NPROC"

echo
echo "编译完成！"

# 提示用户安装
echo
echo "要安装程序，运行: sudo make install"
echo "要运行程序，执行: ./xiaozhi-daemon --config ~/.config/xiaozhi/config.json"
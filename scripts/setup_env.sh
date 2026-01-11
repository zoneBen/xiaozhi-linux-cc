#!/bin/bash

# 小智AI Linux版环境设置脚本
# 用于设置必要的环境变量和路径

echo "小智AI Linux版环境设置脚本"

# 检测系统架构
ARCH=$(uname -m)
echo "系统架构: $ARCH"

# 检测操作系统
if [ -f /etc/os-release ]; then
    . /etc/os-release
    DISTRO=$NAME
    DISTRO_ID=$ID
else
    DISTRO="Unknown"
    DISTRO_ID="unknown"
fi

echo "操作系统: $DISTRO ($DISTRO_ID)"

# 设置PKG_CONFIG_PATH以包含/usr/local/lib/pkgconfig
echo "设置PKG_CONFIG_PATH环境变量..."
export PKG_CONFIG_PATH="/usr/local/lib/pkgconfig:$PKG_CONFIG_PATH"

# 检查是否已存在.bashrc中的设置
if ! grep -q "PKG_CONFIG_PATH=/usr/local/lib/pkgconfig" ~/.bashrc; then
    echo "将PKG_CONFIG_PATH添加到~/.bashrc中..."
    echo 'export PKG_CONFIG_PATH="/usr/local/lib/pkgconfig:$PKG_CONFIG_PATH"' >> ~/.bashrc
    echo "已添加PKG_CONFIG_PATH到~/.bashrc"
else
    echo "PKG_CONFIG_PATH已在~/.bashrc中设置"
fi

# 检查重要的库是否存在
echo "检查重要库文件..."
LIBS=("alsa" "openssl" "json-c" "opus" "opusenc" "opusfile")

MISSING_LIBS=()
for lib in "${LIBS[@]}"; do
    if pkg-config --exists "$lib"; then
        echo "  ✓ $lib"
    else
        echo "  ✗ $lib"
        MISSING_LIBS+=("$lib")
    fi
done

if [ ${#MISSING_LIBS[@]} -eq 0 ]; then
    echo
    echo "✓ 所有依赖库都已找到！"
else
    echo
    echo "⚠ 以下库未找到: ${MISSING_LIBS[*]}"
    echo
    echo "请参考 INSTALL.md 文档安装缺失的依赖库。"
fi

# 检查重要工具
echo
echo "检查构建工具..."
TOOLS=("gcc" "g++" "make" "cmake" "pkg-config" "git")

MISSING_TOOLS=()
for tool in "${TOOLS[@]}"; do
    if command -v "$tool" &> /dev/null; then
        echo "  ✓ $tool"
    else
        echo "  ✗ $tool"
        MISSING_TOOLS+=("$tool")
    fi
done

if [ ${#MISSING_TOOLS[@]} -eq 0 ]; then
    echo
    echo "✓ 所有构建工具都已安装！"
else
    echo
    echo "⚠ 以下工具未安装: ${MISSING_TOOLS[*]}"
    echo
    if [[ "$DISTRO_ID" == "ubuntu" || "$DISTRO_ID" == "debian" ]]; then
        echo "安装命令: sudo apt install build-essential cmake pkg-config git"
    elif [[ "$DISTRO_ID" == "centos" || "$DISTRO_ID" == "rhel" || "$DISTRO_ID" == "fedora" ]]; then
        echo "安装命令: sudo yum install gcc gcc-c++ make cmake pkg-config git"
    fi
fi

echo
echo "环境设置完成！"
echo "当前PKG_CONFIG_PATH: $PKG_CONFIG_PATH"
echo
echo "要立即应用更改，请运行: source ~/.bashrc"
echo "或者重新打开终端窗口。"
#!/bin/bash

# 小智AI Linux版依赖检查脚本
# 自动检测系统类型并提供相应的包安装命令

echo "小智AI Linux版依赖检查脚本"
echo

# 检测Linux发行版
detect_distro() {
    if [ -f /etc/os-release ]; then
        . /etc/os-release
        DISTRO=$NAME
        DISTRO_ID=$ID
    elif type lsb_release >/dev/null 2>&1; then
        DISTRO=$(lsb_release -si)
        DISTRO_ID=$(echo $DISTRO | tr '[:upper:]' '[:lower:]')
    else
        DISTRO=$(uname -s)
        DISTRO_ID="unknown"
    fi
    
    echo "检测到发行版: $DISTRO ($DISTRO_ID)"
}

# 检查命令是否存在
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# 检查包是否安装（基于dpkg或rpm）
package_installed() {
    if command_exists dpkg; then
        dpkg -l | grep -q "^ii  $1 "
    elif command_exists rpm; then
        rpm -q "$1" >/dev/null 2>&1
    else
        # fallback: check with pkg-config
        pkg-config --exists "$1" 2>/dev/null
    fi
}

# 主要检查函数
check_dependencies() {
    echo "检查依赖..."
    echo
    
    # 检查构建工具
    echo "构建工具检查:"
    for tool in gcc g++ make cmake pkg-config; do
        if command_exists $tool; then
            echo "  ✓ $tool"
        else
            echo "  ✗ $tool"
        fi
    done
    
    echo
    
    # 检查库
    echo "库文件检查:"
    for lib in alsa openssl json-c opus opusenc opusfile; do
        if pkg-config --exists $lib; then
            echo "  ✓ $lib"
        else
            echo "  ✗ $lib"
        fi
    done
    
    echo
}

# 提供安装命令
provide_install_commands() {
    echo "根据检测到的系统，安装依赖的命令如下："
    echo
    
    case $DISTRO_ID in
        ubuntu|debian|mint)
            echo "Ubuntu/Debian 系统:"
            echo "  sudo apt update"
            echo "  sudo apt install build-essential alsa-utils libasound2-dev libssl-dev \\"
            echo "                 libjson-c-dev libopus-dev libopusenc-dev libopusfile-dev \\"
            echo "                 cmake pkg-config"
            echo
            echo "如果遇到 'E: Unable to locate package libopusenc-dev' 错误，请尝试:"
            echo "  sudo apt update && sudo apt install libopusenc-dev"
            echo "或者:"
            echo "  sudo apt install opus-tools libopus-dev libopusenc-dev libopusfile-dev"
            ;;
        centos|rhel|fedora|rocky|almalinux)
            echo "CentOS/RHEL/Fedora 系统:"
            echo "  sudo yum install gcc gcc-c++ make alsa-lib-devel openssl-devel \\"
            echo "                 json-c-devel opus-devel opus-tools cmake pkg-config"
            ;;
        arch|manjaro)
            echo "Arch Linux/Manjaro 系统:"
            echo "  sudo pacman -S base-devel alsa-lib json-c opus cmake pkg-config"
            ;;
        opensuse*|sles)
            echo "openSUSE/SLES 系统:"
            echo "  sudo zypper install gcc gcc-c++ make alsa-devel libopenssl-devel \\"
            echo "                      json-c-devel opus-devel opus-tools cmake pkg-config"
            ;;
        *)
            echo "未知系统，尝试通用命令:"
            echo "Ubuntu/Debian: sudo apt install build-essential alsa-utils libasound2-dev libssl-dev libjson-c-dev libopus-dev libopusenc-dev libopusfile-dev cmake pkg-config"
            echo "CentOS/RHEL/Fedora: sudo yum install gcc gcc-c++ make alsa-lib-devel openssl-devel json-c-devel opus-devel opus-tools cmake pkg-config"
            ;;
    esac
}

# 主程序
main() {
    detect_distro
    echo
    check_dependencies
    provide_install_commands
}

main "$@"
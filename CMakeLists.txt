cmake_minimum_required(VERSION 3.16)
project(xiaozhi-linux-cc VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找依赖库
find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)

# ALSA音频库
find_library(ALSA_LIBRARY asound)
find_path(ALSA_INCLUDE_DIR NAMES alsa/asoundlib.h)

# OpenSSL
find_package(OpenSSL REQUIRED)

# JSON-C
pkg_check_modules(JSONC REQUIRED json-c)

# Opus
pkg_check_modules(OPUS REQUIRED opus)
pkg_check_modules(OPUSENC REQUIRED opusenc)
pkg_check_modules(OPUSFILE REQUIRED opusfile)

# 包含目录
include_directories(
    include
    src
)

# 定义源文件
set(AUDIO_SOURCES
    src/audio/audio_manager.cpp
    src/audio/alsa_handler.cpp
    src/audio/opus_encoder.cpp
    src/audio/opus_decoder.cpp
)

set(NETWORK_SOURCES
    src/network/websocket_client.cpp
    src/network/mqtt_client.cpp
    src/network/protocol_handler.cpp
)

set(MCP_SOURCES
    src/mcp/mcp_server.cpp
    src/mcp/tool_registry.cpp
    src/mcp/json_rpc_handler.cpp
)

set(AI_SOURCES
    src/ai/ai_engine.cpp
    src/ai/conversation_manager.cpp
)

set(CLI_SOURCES
    src/cli/command_handler.cpp
)

set(UTILS_SOURCES
    src/utils/config_manager.cpp
    src/utils/logger.cpp
)

# 创建可执行文件
add_executable(xiaozhi-daemon
    src/main.cpp
    ${AUDIO_SOURCES}
    ${NETWORK_SOURCES}
    ${MCP_SOURCES}
    ${AI_SOURCES}
    ${CLI_SOURCES}
    ${UTILS_SOURCES}
)

# 链接库
target_link_libraries(xiaozhi-daemon
    ${ALSA_LIBRARY}
    ${OPENSSL_LIBRARIES}
    ${JSONC_LIBRARIES}
    ${OPUS_LIBRARIES}
    ${OPUSENC_LIBRARIES}
    ${OPUSFILE_LIBRARIES}
    Threads::Threads
    -ldl
)

# 安装规则
install(TARGETS xiaozhi-daemon
    RUNTIME DESTINATION bin
)

install(DIRECTORY configs/
    DESTINATION etc/xiaozhi
    FILES_MATCHING PATTERN "*.json"
)

install(FILES systemd/xiaozhi.service
    DESTINATION lib/systemd/system/
)

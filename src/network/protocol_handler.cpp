#include "protocol_handler.h"
#include <iostream>
#include <json-c/json.h>

namespace xiaozhi {

ProtocolHandler::ProtocolHandler() {
    std::cout << "[ProtocolHandler] 初始化协议处理器" << std::endl;
}

ProtocolHandler::~ProtocolHandler() {
    std::cout << "[ProtocolHandler] 协议处理器已销毁" << std::endl;
}

std::string ProtocolHandler::handleWebSocketMessage(const std::string& message) {
    // 解析WebSocket消息
    json_object* jobj = json_tokener_parse(message.c_str());
    if (!jobj) {
        std::cerr << "[ProtocolHandler] 错误: 无法解析JSON消息" << std::endl;
        return createErrorMessage("Invalid JSON");
    }

    std::string response;
    json_object* type_obj = nullptr;
    if (json_object_object_get_ex(jobj, "type", &type_obj)) {
        std::string type = json_object_get_string(type_obj);
        
        if (type == "hello") {
            response = handleHello(message);
        } else if (type == "listen") {
            response = handleListen(message);
        } else if (type == "stt") {
            response = handleStt(message);
        } else if (type == "tts") {
            response = handleTts(message);
        } else if (type == "mcp") {
            response = handleMcp(message);
        } else {
            response = createErrorMessage("Unknown message type: " + type);
        }
    } else {
        response = createErrorMessage("Missing message type");
    }

    json_object_put(jobj); // 释放内存
    return response;
}

std::string ProtocolHandler::handleHello(const std::string& message) {
    json_object* jobj = json_tokener_parse(message.c_str());
    
    std::string response = "{"
        "\"type\":\"hello\","
        "\"accepted\":true,"
        "\"sessionId\":\"session_" + std::to_string(rand()) + "\","
        "\"serverVersion\":\"1.0.0\""
        "}";
    
    if (jobj) json_object_put(jobj);
    return response;
}

std::string ProtocolHandler::handleListen(const std::string& message) {
    json_object* jobj = json_tokener_parse(message.c_str());
    
    json_object* listening_obj = nullptr;
    bool listening = false;
    if (jobj && json_object_object_get_ex(jobj, "listening", &listening_obj)) {
        listening = json_object_get_boolean(listening_obj);
    }
    
    std::string action = listening ? "started" : "stopped";
    std::string response = "{"
        "\"type\":\"listen\","
        "\"status\":\"" + action + "\""
        "}";
    
    if (jobj) json_object_put(jobj);
    return response;
}

std::string ProtocolHandler::handleStt(const std::string& message) {
    json_object* jobj = json_tokener_parse(message.c_str());
    
    std::string response = "{"
        "\"type\":\"stt\","
        "\"text\":\"Simulated speech recognition result\","
        "\"confidence\":0.95"
        "}";
    
    if (jobj) json_object_put(jobj);
    return response;
}

std::string ProtocolHandler::handleTts(const std::string& message) {
    json_object* jobj = json_tokener_parse(message.c_str());
    
    std::string response = "{"
        "\"type\":\"tts\","
        "\"status\":\"processed\""
        "}";
    
    if (jobj) json_object_put(jobj);
    return response;
}

std::string ProtocolHandler::handleMcp(const std::string& message) {
    json_object* jobj = json_tokener_parse(message.c_str());
    
    std::string response = "{"
        "\"type\":\"mcp\","
        "\"response\":\"MCP command processed\""
        "}";
    
    if (jobj) json_object_put(jobj);
    return response;
}

std::string ProtocolHandler::createErrorMessage(const std::string& error_msg) {
    std::string response = "{"
        "\"type\":\"error\","
        "\"message\":\"" + error_msg + "\""
        "}";
    return response;
}

void ProtocolHandler::setResponseCallback(std::function<void(const std::string&)> callback) {
    response_callback_ = callback;
}

} // namespace xiaozhi
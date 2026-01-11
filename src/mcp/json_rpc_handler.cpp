#include "json_rpc_handler.h"
#include <iostream>
#include <json-c/json.h>
#include <sstream>
#include <random>

namespace xiaozhi {

JsonRpcHandler::JsonRpcHandler(ToolRegistry& registry) : tool_registry_(registry) {
    std::cout << "[JsonRpcHandler] 初始化JSON-RPC处理器" << std::endl;
}

JsonRpcHandler::~JsonRpcHandler() {
    std::cout << "[JsonRpcHandler] JSON-RPC处理器已销毁" << std::endl;
}

std::string JsonRpcHandler::handleRequest(const std::string& request) {
    json_object* jobj = json_tokener_parse(request.c_str());
    if (!jobj) {
        std::cerr << "[JsonRpcHandler] 错误: 无法解析JSON-RPC请求" << std::endl;
        return createErrorResponse(-32700, "Parse error", nullptr);
    }

    // 检查必需字段
    json_object* jsonrpc_obj = nullptr;
    json_object* method_obj = nullptr;
    json_object* id_obj = nullptr;
    
    if (!json_object_object_get_ex(jobj, "jsonrpc", &jsonrpc_obj)) {
        json_object_put(jobj);
        return createErrorResponse(-32600, "Invalid Request: Missing jsonrpc field", nullptr);
    }
    
    std::string jsonrpc_version = json_object_get_string(jsonrpc_obj);
    if (jsonrpc_version != "2.0") {
        json_object_put(jobj);
        return createErrorResponse(-32600, "Invalid Request: Unsupported jsonrpc version", nullptr);
    }
    
    if (!json_object_object_get_ex(jobj, "method", &method_obj)) {
        json_object_put(jobj);
        return createErrorResponse(-32600, "Invalid Request: Missing method", nullptr);
    }
    
    std::string method = json_object_get_string(method_obj);
    id_obj = nullptr;
    json_object_object_get_ex(jobj, "id", &id_obj); // id is optional for notifications

    std::string response;
    if (method == "initialize") {
        response = handleInitialize(jobj);
    } else if (method == "tools/list") {
        response = handleListTools(jobj);
    } else if (method == "tools/call") {
        response = handleCallTool(jobj);
    } else {
        response = createErrorResponse(-32601, "Method not found: " + method, id_obj);
    }

    json_object_put(jobj);
    return response;
}

std::string JsonRpcHandler::handleInitialize(json_object* request) {
    json_object* id_obj = nullptr;
    json_object_object_get_ex(request, "id", &id_obj);
    
    std::string response = "{"
        "\"jsonrpc\":\"2.0\","
        "\"result\":{"
            "\"protocolVersion\":\"1.0\","
            "\"serverInfo\":{"
                "\"name\":\"xiaozhi-mcp-server\","
                "\"version\":\"1.0.0\""
            "},"
            "\"capabilities\":{"
                "\"tools\":true,"
                "\"experimental\":{}"
            "}"
        "}";
    
    if (id_obj) {
        // 如果有ID，则返回完整响应
        json_object* resp_obj = json_tokener_parse(response.c_str());
        json_object_object_add(resp_obj, "id", json_object_get(id_obj)); // 增加引用计数
        std::string result = json_object_to_json_string_ext(resp_obj, JSON_C_TO_STRING_PLAIN);
        json_object_put(resp_obj);
        return result;
    } else {
        // 如果没有ID，则这是通知，只返回result部分
        return response;
    }
}

std::string JsonRpcHandler::handleListTools(json_object* request) {
    json_object* id_obj = nullptr;
    json_object_object_get_ex(request, "id", &id_obj);
    
    auto tools = tool_registry_.getAllTools();
    
    json_object* result_obj = json_object_new_object();
    json_object* tools_array = json_object_new_array();
    
    for (const auto& tool : tools) {
        json_object* tool_obj = json_object_new_object();
        json_object_object_add(tool_obj, "name", json_object_new_string(tool.name.c_str()));
        json_object_object_add(tool_obj, "description", json_object_new_string(tool.description.c_str()));
        
        // 创建参数schema
        json_object* schema_obj = json_object_new_object();
        json_object_object_add(schema_obj, "type", json_object_new_string("object"));
        
        json_object* properties_obj = json_object_new_object();
        for (const auto& param : tool.parameters) {
            json_object* param_obj = json_object_new_object();
            json_object_object_add(param_obj, "type", json_object_new_string(param.type.c_str()));
            json_object_object_add(param_obj, "description", json_object_new_string(param.description.c_str()));
            json_object_object_add(properties_obj, param.name.c_str(), param_obj);
        }
        json_object_object_add(schema_obj, "properties", properties_obj);
        
        // 必需参数
        json_object* required_array = json_object_new_array();
        for (const auto& param : tool.parameters) {
            if (param.required) {
                json_object_array_add(required_array, json_object_new_string(param.name.c_str()));
            }
        }
        json_object_object_add(schema_obj, "required", required_array);
        
        json_object_object_add(tool_obj, "inputSchema", schema_obj);
        json_object_array_add(tools_array, tool_obj);
    }
    
    json_object_object_add(result_obj, "tools", tools_array);
    
    std::string response = "{"
        "\"jsonrpc\":\"2.0\","
        "\"result\":" + std::string(json_object_to_json_string_ext(result_obj, JSON_C_TO_STRING_PLAIN)) + 
        "}";
    
    if (id_obj) {
        json_object* resp_obj = json_tokener_parse(response.c_str());
        json_object_object_add(resp_obj, "id", json_object_get(id_obj));
        std::string result = json_object_to_json_string_ext(resp_obj, JSON_C_TO_STRING_PLAIN);
        json_object_put(resp_obj);
        json_object_put(result_obj);
        return result;
    } else {
        std::string result = json_object_to_json_string_ext(result_obj, JSON_C_TO_STRING_PLAIN);
        json_object_put(result_obj);
        return "{\"jsonrpc\":\"2.0\",\"result\":" + result + "}";
    }
}

std::string JsonRpcHandler::handleCallTool(json_object* request) {
    json_object* id_obj = nullptr;
    json_object_object_get_ex(request, "id", &id_obj);
    
    json_object* params_obj = nullptr;
    if (!json_object_object_get_ex(request, "params", &params_obj)) {
        return createErrorResponse(-32602, "Invalid params: Missing params field", id_obj);
    }
    
    json_object* name_obj = nullptr;
    if (!json_object_object_get_ex(params_obj, "name", &name_obj)) {
        return createErrorResponse(-32602, "Invalid params: Missing tool name", id_obj);
    }
    
    std::string tool_name = json_object_get_string(name_obj);
    
    // 解析参数
    json_object* arguments_obj = nullptr;
    std::map<std::string, std::string> arguments;
    if (json_object_object_get_ex(params_obj, "arguments", &arguments_obj)) {
        json_object_object_foreach(arguments_obj, key, val) {
            arguments[key] = json_object_get_string(val);
        }
    }
    
    // 调用工具
    std::string result = tool_registry_.callTool(tool_name, arguments);
    
    std::string response = "{"
        "\"jsonrpc\":\"2.0\","
        "\"result\":" + result +
        "}";
    
    if (id_obj) {
        json_object* resp_obj = json_tokener_parse(response.c_str());
        json_object_object_add(resp_obj, "id", json_object_get(id_obj));
        std::string final_response = json_object_to_json_string_ext(resp_obj, JSON_C_TO_STRING_PLAIN);
        json_object_put(resp_obj);
        return final_response;
    } else {
        return response;
    }
}

std::string JsonRpcHandler::createErrorResponse(int code, const std::string& message, json_object* id_obj) {
    json_object* error_obj = json_object_new_object();
    json_object_object_add(error_obj, "code", json_object_new_int(code));
    json_object_object_add(error_obj, "message", json_object_new_string(message.c_str()));
    
    std::string response = "{"
        "\"jsonrpc\":\"2.0\","
        "\"error\":" + std::string(json_object_to_json_string_ext(error_obj, JSON_C_TO_STRING_PLAIN)) + 
        "}";
    
    if (id_obj) {
        json_object* resp_obj = json_tokener_parse(response.c_str());
        json_object_object_add(resp_obj, "id", json_object_get(id_obj));
        std::string result = json_object_to_json_string_ext(resp_obj, JSON_C_TO_STRING_PLAIN);
        json_object_put(resp_obj);
        json_object_put(error_obj);
        return result;
    } else {
        json_object_put(error_obj);
        return response;
    }
}

} // namespace xiaozhi
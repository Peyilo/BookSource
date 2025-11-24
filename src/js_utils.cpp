//
// Created by Peyilo on 2025/11/24.
//

#include "js_utils.h"
#include <quickjs/quickjs.h>
#include <iostream>

std::string run_simple_js(const std::string& code) {
    // 创建 runtime
    JSRuntime *rt = JS_NewRuntime();
    if (!rt) throw std::runtime_error("Failed to create JSRuntime");

    // 创建 context
    JSContext *ctx = JS_NewContext(rt);
    if (!ctx) {
        JS_FreeRuntime(rt);
        throw std::runtime_error("Failed to create JSContext");
    }

    // 执行脚本
    const JSValue val = JS_Eval(ctx, code.c_str(), code.size(), "<eval>", JS_EVAL_TYPE_GLOBAL);

    if (JS_IsException(val)) {
        const JSValue err = JS_GetException(ctx);
        const char *errStr = JS_ToCString(ctx, err);

        const std::string message = errStr ? errStr : "Unknown JS error";

        JS_FreeCString(ctx, errStr);
        JS_FreeValue(ctx, err);
        JS_FreeValue(ctx, val);
        JS_FreeContext(ctx);
        JS_FreeRuntime(rt);

        throw std::runtime_error(message);
    }

    // 转成字符串
    const char *resStr = JS_ToCString(ctx, val);
    std::string result = resStr ? resStr : "";

    // 释放 JS 字符串
    JS_FreeCString(ctx, resStr);
    JS_FreeValue(ctx, val);

    // 销毁 VM
    JS_FreeContext(ctx);
    JS_FreeRuntime(rt);

    return result;
}

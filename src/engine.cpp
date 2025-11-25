#include <booksource/engine.h>
#include <iostream>

QuickJsEngine::QuickJsEngine() {
    runtime = JS_NewRuntime();
    if (!runtime) throw std::runtime_error("Failed to create JSRuntime");

    context = JS_NewContext(runtime);
    if (!context) {
        JS_FreeRuntime(runtime);
        throw std::runtime_error("Failed to create JSContext");
    }
}

QuickJsEngine::~QuickJsEngine() {
    if (context) JS_FreeContext(context);
    if (runtime) JS_FreeRuntime(runtime);
}

std::string QuickJsEngine::eval(const std::string &code) const {
    const JSValue val = JS_Eval(context, code.c_str(), code.size(),
                                "<eval>", JS_EVAL_TYPE_GLOBAL);

    if (JS_IsException(val)) {
        const JSValue err = JS_GetException(context);
        const char *errStr = JS_ToCString(context, err);

        const std::string message = errStr ? errStr : "Unknown JS error";
        JS_FreeCString(context, errStr);
        JS_FreeValue(context, err);
        JS_FreeValue(context, val);
        throw std::runtime_error(message);
    }

    const char *resStr = JS_ToCString(context, val);
    std::string result = resStr ? resStr : "";

    JS_FreeCString(context, resStr);
    JS_FreeValue(context, val);
    return result;
}

void QuickJsEngine::reset() {
    if (context) JS_FreeContext(context);
    context = JS_NewContext(runtime);

    if (!context) {
        throw std::runtime_error("Failed to recreate JSContext during reset()");
    }
}

void QuickJsEngine::addValue(const std::string &name, const std::string &value) const {
    const JSValue global = JS_GetGlobalObject(context);
    const JSValue str = JS_NewString(context, value.c_str());
    JS_SetPropertyStr(context, global, name.c_str(), str);
    JS_FreeValue(context, global);
}

void QuickJsEngine::addValue(const std::string &name, const char *value) const {
    addValue(name, std::string(value));
}

void QuickJsEngine::addValue(const std::string &name, const int value) const {
    const JSValue global = JS_GetGlobalObject(context);
    const JSValue num = JS_NewInt32(context, value);
    JS_SetPropertyStr(context, global, name.c_str(), num);
    JS_FreeValue(context, global);
}

void QuickJsEngine::addValue(const std::string &name, const double value) const {
    const JSValue global = JS_GetGlobalObject(context);
    const JSValue num = JS_NewFloat64(context, value);
    JS_SetPropertyStr(context, global, name.c_str(), num);
    JS_FreeValue(context, global);
}

void QuickJsEngine::addValue(const std::string &name, const bool value) const {
    const JSValue global = JS_GetGlobalObject(context);
    const JSValue b = JS_NewBool(context, value);
    JS_SetPropertyStr(context, global, name.c_str(), b);
    JS_FreeValue(context, global);
}

// 供js中调用的屏幕输出函数
static JSValue js_print(
    JSContext* ctx,
    JSValueConst _,
    const int argc,
    JSValueConst* argv) {
    for (int i = 0; i < argc; i++) {
        const char* str = JS_ToCString(ctx, argv[i]);
        if (!str)
            return JS_EXCEPTION;

        std::cout << str;
        JS_FreeCString(ctx, str);

        if (i + 1 < argc)
            std::cout << " ";
    }

    std::cout << std::endl;
    return JS_UNDEFINED;
}

void QuickJsEngine::addPrintFunc(const std::string &funcName) const {
    const JSValue global = JS_GetGlobalObject(context);
    JS_SetPropertyStr(context,global,
        funcName.c_str(),JS_NewCFunction(context, js_print, funcName.c_str(), 1)
    );
    JS_FreeValue(context, global);
}

void QuickJsEngine::deleteValue(const std::string &name) {
    if (!context)
        throw std::runtime_error("QuickJsEngine: context is null");

    const JSValue global = JS_GetGlobalObject(context);
    const JSAtom atom = JS_NewAtom(context, name.c_str());

    // 删除属性
    JS_DeleteProperty(context, global, atom, JS_PROP_THROW);


    // 释放 atom 和 global
    JS_FreeAtom(context, atom);
    JS_FreeValue(context, global);
}

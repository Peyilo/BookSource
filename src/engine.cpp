#include <booksource/engine.h>

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

void QuickJsEngine::addBinding(const std::string &name, const std::string &value) const {
    const JSValue global = JS_GetGlobalObject(context);
    const JSValue str = JS_NewString(context, value.c_str());
    JS_SetPropertyStr(context, global, name.c_str(), str);
    JS_FreeValue(context, global);
}

void QuickJsEngine::addBinding(const std::string &name, const int value) const {
    const JSValue global = JS_GetGlobalObject(context);
    const JSValue num = JS_NewInt32(context, value);
    JS_SetPropertyStr(context, global, name.c_str(), num);
    JS_FreeValue(context, global);
}

void QuickJsEngine::addBinding(const std::string &name, const double value) const {
    const JSValue global = JS_GetGlobalObject(context);
    const JSValue num = JS_NewFloat64(context, value);
    JS_SetPropertyStr(context, global, name.c_str(), num);
    JS_FreeValue(context, global);
}

void QuickJsEngine::addBinding(const std::string &name, const bool value) const {
    const JSValue global = JS_GetGlobalObject(context);
    const JSValue b = JS_NewBool(context, value);
    JS_SetPropertyStr(context, global, name.c_str(), b);
    JS_FreeValue(context, global);
}


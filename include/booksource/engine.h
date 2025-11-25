#pragma once

#include <stdexcept>
#include <quickjs/quickjs.h>
#include <string>
#include <booksource/js_binder.h>

class QuickJsEngine {
public:
    QuickJsEngine();
    ~QuickJsEngine();

    /// 执行脚本并返回字符串结果
    std::string eval(const std::string &code) const;

    /// 重新创建上下文（清空状态）
    void reset();

    /// 添加全局变量绑定
    void addBinding(const std::string &name, const std::string &value) const;
    void addBinding(const std::string& name, int value) const;
    void addBinding(const std::string& name, double value) const;
    void addBinding(const std::string& name, bool value) const;

    template<typename T>
    void addObject(const std::string &name, T *instance) const {
        if (!context)
            throw std::runtime_error("QuickJsEngine: context is null");

        const JSValue global = JS_GetGlobalObject(context);
        const JSValue obj    = JsBinder<T>::wrap(context, instance);

        if (JS_IsException(obj)) {
            JS_FreeValue(context, global);
            throw std::runtime_error("QuickJsEngine: failed to wrap object");
        }

        JS_SetPropertyStr(context, global, name.c_str(), obj);
        JS_FreeValue(context, global);
    }

    JSContext* getContext() const { return context; }

private:
    JSRuntime *runtime = nullptr;
    JSContext *context = nullptr;
};
#pragma once

#include <shared_mutex>
#include <quickjs/quickjs.h>
#include <string>
#include <booksource/bind.h>

class QuickJsEngine {
public:
    QuickJsEngine();

    ~QuickJsEngine();

    /// 执行脚本并返回字符串结果
    std::string eval(const std::string &code) const;

    /// 重新创建上下文（清空状态）
    void reset();

    void addValue(const std::string &name, const std::string &value) const;

    void addValue(const std::string &name, const char *value) const;

    void addValue(const std::string &name, int value) const;

    void addValue(const std::string &name, double value) const;

    void addValue(const std::string &name, bool value) const;

    template<typename T>
    void addValueBinding(const std::string &name, T *ptr) {
        if (!context)
            throw std::runtime_error("QuickJsEngine: context is null");

        JSValue global = JS_GetGlobalObject(context);

        // 1. 每个引擎获得属于自己的 index
        const int id = const_cast<QuickJsEngine *>(this)->registerBoundPtr(ptr);
        nameToPtrIndex[name] = id; // 记录 name → id

        JS_DefinePropertyGetSet(
            context,
            global,
            JS_NewAtom(context, name.c_str()),

            // getter
            JS_NewCFunctionMagic(
                context,
                [](JSContext *ctx, JSValueConst this_val, int, JSValueConst *, int magic) {
                    const QuickJsEngine *engine = fromContext(ctx);
                    if (!engine)
                        return JS_ThrowInternalError(ctx, "Engine not found");

                    T *p = static_cast<T *>(engine->getBoundPtr(magic));
                    return JSConverter<T>::toJS(ctx, *p);
                },
                name.c_str(),
                0,
                JS_CFUNC_generic_magic,
                id),

            // setter
            JS_NewCFunctionMagic(
                context,
                [](JSContext *ctx, JSValueConst this_val, int, JSValueConst *argv, int magic) {
                    const QuickJsEngine *engine = fromContext(ctx);
                    if (!engine)
                        return JS_ThrowInternalError(ctx, "Engine not found");

                    T *p = static_cast<T *>(engine->getBoundPtr(magic));
                    *p = JSConverter<T>::fromJS(ctx, argv[0]);
                    return JS_UNDEFINED;
                },
                name.c_str(),
                1,
                JS_CFUNC_generic_magic,
                id),

            JS_PROP_ENUMERABLE | JS_PROP_CONFIGURABLE
        );

        JS_FreeValue(context, global);
    }

    /// 添加全局变量绑定
    /// 注意：如果需要给对象绑定字段以及方法，请在调用改函数之前保证字段表、函数表已经完成了初始化
    template<typename T>
    void addObjectBinding(const std::string &name, T *instance) const {
        if (!context)
            throw std::runtime_error("QuickJsEngine: context is null");

        const JSValue global = JS_GetGlobalObject(context);
        const JSValue obj = JsBinder<T>::bind(context, instance);

        if (JS_IsException(obj)) {
            JS_FreeValue(context, global);
            throw std::runtime_error("QuickJsEngine: failed to wrap object");
        }

        JS_SetPropertyStr(context, global, name.c_str(), obj);
        JS_FreeValue(context, global);
    }

    // 从js当前环境中删除指定变量
    void deleteValue(const std::string &name);

    // 添加一个全局函数：用于屏幕打印输出
    void addPrintFunc(const std::string &funcName) const;

    JSContext *getContext() const { return context; }

    int getEngineId() const { return engineID; }

    // 通过 JSContext 反查对应的 QuickJsEngine*
    static QuickJsEngine *fromContext(JSContext *ctx);

    static QuickJsEngine *getEngineById(int id);

private:
    JSRuntime *runtime = nullptr;
    JSContext *context = nullptr;
    int engineID = 0;

    std::unordered_map<int, void *> boundPtrTable;
    int nextPtrTableId = 0;
    std::unordered_map<std::string, int> nameToPtrIndex;

    // 注册一个指针，返回索引
    int registerBoundPtr(void *p) {
        const auto id = nextPtrTableId++;
        boundPtrTable[id] = p;
        return id;
    }

    // 通过索引拿指针
    void *getBoundPtr(const int id) const {
        const auto it = boundPtrTable.find(id);
        if (it == boundPtrTable.end())
            return nullptr;
        return it->second;
    }

private:
    // 下一个js引擎的ID（全局唯一）
    static std::atomic<int> s_nextEngineId;
    static std::shared_mutex s_engineRegistryMutex;
    static std::unordered_map<int, QuickJsEngine *> s_engineRegistry;
};

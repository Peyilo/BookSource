#pragma once

#include <stdexcept>
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
    void addValueBinding(const std::string &name, T *ptr) const {
        if (!context)
            throw std::runtime_error("QuickJsEngine: context is null");

        JSValue global = JS_GetGlobalObject(context);

        // 在全局表里存一份指针，返回一个 int 索引
        const int id = registerBoundPtr(ptr);

        JS_DefinePropertyGetSet(
            context,
            global,
            JS_NewAtom(context, name.c_str()),

            // getter
            JS_NewCFunctionMagic(
                context,
                [](JSContext *ctx, JSValueConst this_val,
                   int, JSValueConst *, int magic) {
                    // 通过 magic 拿指针
                    void *raw = QuickJsEngine::getBoundPtr(magic);
                    T *p = static_cast<T *>(raw);
                    return JSConverter<T>::toJS(ctx, *p);
                },
                name.c_str(),
                0,
                JS_CFUNC_generic_magic,
                id),

            // setter
            JS_NewCFunctionMagic(
                context,
                [](JSContext *ctx, JSValueConst this_val,
                   int, JSValueConst *argv, int magic) {
                    void *raw = QuickJsEngine::getBoundPtr(magic);
                    T *p = static_cast<T *>(raw);
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

private:
    JSRuntime *runtime = nullptr;
    JSContext *context = nullptr;

    // TODO: 换成每个实例都有一个
    // 全局指针表：专门为 magic 做索引
    static std::vector<void*> &boundPtrTable() {
        static std::vector<void*> table;
        return table;
    }

    // 注册一个指针，返回索引
    static int registerBoundPtr(void *p) {
        auto &tbl = boundPtrTable();
        tbl.push_back(p);
        return static_cast<int>(tbl.size() - 1);
    }

    static void *getBoundPtr(int id) {
        auto &tbl = boundPtrTable();
        if (id < 0 || static_cast<size_t>(id) >= tbl.size())
            return nullptr;
        return tbl[id];
    }
};

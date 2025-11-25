#pragma once

#include <quickjs/quickjs.h>
#include <functional>
#include <string>
#include <vector>

/**
 * 通用绑定器：
 *  - 对每个 T 维护一套 Field / Method 表
 *  - wrap(ctx, T*) 时，根据表自动创建 JS 对象
 *  - 方法回调交给调用者实现（返回 JSValue）
 *
 * 使用方式见 quickjs_engine.h 中的示例。
 */
template<typename T>
class JsBinder {
public:
    using MethodFunc = std::function<JSValue(JSContext *ctx,
                                             T *instance,
                                             int argc,
                                             JSValueConst *argv)>;

    using FieldGetter = std::function<std::string(T *instance)>;

    struct Method {
        std::string name;
        MethodFunc  func;
    };

    struct Field {
        std::string  name;
        FieldGetter  getter;
    };

    /// 注册一个字段（在 wrap() 时快照为字符串属性）
    static void addField(const std::string &name, FieldGetter getter);

    /// 注册一个方法（wrap() 时挂到对象上）
    static void addMethod(const std::string &name, MethodFunc func);

    /// 把原生对象包装成 JS 对象（**不负责释放 T* 的生命周期**）
    static JSValue wrap(JSContext *ctx, T *instance);

    /// 可选：设置类名，仅用于调试/错误信息
    static void setClassName(const std::string &name);

private:
    static void ensureClassInit(JSRuntime *rt);

    static JSValue methodDispatcher(JSContext *ctx,
                                    JSValueConst this_val,
                                    int argc,
                                    JSValueConst *argv,
                                    int magic);

    static void finalizer(JSRuntime *rt, JSValue val);

private:
    static inline JSClassID             s_classId { 0 };
    static inline bool                 s_inited   { false };
    static inline std::string          s_className { "NativeObject" };
    static inline std::vector<Field>   s_fields   {};
    static inline std::vector<Method>  s_methods  {};
};

#include "js_binder.inl"
#pragma once
#include <quickjs/quickjs.h>
#include <string>
#include <functional>
#include <vector>

template<typename T>
struct JSConverter;

// int
template<>
struct JSConverter<int> {
    static int fromJS(JSContext *ctx, JSValueConst v) {
        int32_t out;
        JS_ToInt32(ctx, &out, v);
        return out;
    }

    static JSValue toJS(JSContext *ctx, int v) { return JS_NewInt32(ctx, v); }
};

// double
template<>
struct JSConverter<double> {
    static double fromJS(JSContext *ctx, JSValueConst v) {
        double out;
        JS_ToFloat64(ctx, &out, v);
        return out;
    }

    static JSValue toJS(JSContext *ctx, double v) { return JS_NewFloat64(ctx, v); }
};

// bool
template<>
struct JSConverter<bool> {
    static bool fromJS(JSContext *ctx, JSValueConst v) {
        return JS_ToBool(ctx, v) == 1;
    }

    static JSValue toJS(JSContext *ctx, bool v) { return JS_NewBool(ctx, v); }
};

// std::string
template<>
struct JSConverter<std::string> {
    static std::string fromJS(JSContext *ctx, JSValueConst v) {
        const char *s = JS_ToCString(ctx, v);
        std::string out = s ? s : "";
        JS_FreeCString(ctx, s);
        return out;
    }

    static JSValue toJS(JSContext *ctx, const std::string &v) {
        return JS_NewString(ctx, v.c_str());
    }
};

// 支持 const T& / T& / const T
template<typename T>
struct JSConverter<const T &> : JSConverter<T> {
};

template<typename T>
struct JSConverter<T &> : JSConverter<T> {
};

template<typename T>
struct JSConverter<const T> : JSConverter<T> {
};

template<typename Class>
class JsBinder {
public:
    template<typename FieldType>
    using FieldGetter = std::function<FieldType (Class *instance)>;

    template<typename FieldType>
    using FieldSetter = std::function<void (Class *instance, FieldType value)>;

    /// 注册一个字段（在 wrap() 时快照为字符串属性）
    /// 注册字段：getter + setter
    template<typename FieldType>
    static void addField(const std::string &name,
                         FieldGetter<FieldType> getter,
                         FieldSetter<FieldType> setter) {
        FieldBase base;
        base.name = name;

        // getter 封装成统一类型（返回 JSValue）
        base.getter = [getter](JSContext *ctx, Class *obj) -> JSValue {
            FieldType v = getter(obj);
            return JSConverter<FieldType>::toJS(ctx, v);
        };

        // setter 封装（从 JSValue 转成 FieldType）
        base.setter = [setter](JSContext *ctx, Class *obj, JSValueConst jsVal) {
            FieldType native{};
            native = JSConverter<FieldType>::fromJS(ctx, jsVal);
            setter(obj, native);
        };

        s_fields.push_back(std::move(base));
    }

    template<typename Ret, typename... Args>
    static void addMethod(const std::string &name, std::function<Ret(Class *, Args...)> fn) {
        MethodBase base;
        base.name = name;

        base.invoker = [fn](JSContext *ctx, Class *obj, int argc, JSValueConst *argv) -> JSValue {
            // 参数解包
            return invokeHelper<Ret, Args...>(ctx, obj, fn, argv);
        };

        s_methods.push_back(std::move(base));
    }

    /// 把原生对象包装成 JS 对象（**不负责释放 Class* 的生命周期**）
    static JSValue wrap(JSContext *ctx, Class *instance) {
        JSRuntime *rt = JS_GetRuntime(ctx);
        ensureClassInit(rt);

        const JSValue obj = JS_NewObjectClass(ctx, s_classId);
        if (JS_IsException(obj)) {
            return obj;
        }

        // 只保存指针，不负责 delete
        JS_SetOpaque(obj, instance);

        // 设置字段（快照）
        for (const auto &f: s_fields) {
            JSValue v = f.getter(ctx, instance);
            JS_SetPropertyStr(ctx, obj, f.name.c_str(), v);
        }

        // 绑定方法
        for (int i = 0; i < s_methods.size(); i++) {
            JSValue fn = JS_NewCFunctionMagic(
                ctx,
                &methodDispatcher,
                s_methods[i].name.c_str(),
                0,                        // JS 侧参数数量由真实调用解析
                JS_CFUNC_generic_magic,
                i                         // magic：方法序号
            );
            JS_SetPropertyStr(ctx, obj, s_methods[i].name.c_str(), fn);
        }

        return obj;
    }

    /// 设置类名，仅用于调试/错误信息
    static void setClassName(const std::string &name) {
        s_className = name;
    }

private:
    struct FieldBase {
        std::string name;
        std::function<JSValue(JSContext *, Class *)> getter;
        std::function<void(JSContext *, Class *, JSValueConst)> setter;
    };

    struct MethodBase {
        std::string name;
        std::function<JSValue(JSContext *, Class *, int, JSValueConst *)> invoker;
    };

    // 确保Class已经被注册了
    static void ensureClassInit(JSRuntime *rt) {
        if (s_inited)
            return;

        JS_NewClassID(&s_classId); // 申请一个新的class_id，保证唯一，用于区分不同的class

        JSClassDef def{};
        def.class_name = s_className.c_str();

        if (JS_NewClass(rt, s_classId, &def) < 0) {
            // 这里抛异常比静默失败要好
            throw std::runtime_error("JS_NewClass failed for " + s_className);
        }

        s_inited = true;
    }

    template<typename Ret, typename... Args>
    static JSValue invokeHelper(JSContext *ctx, Class *obj,
                                std::function<Ret(Class *, Args...)> &fn,
                                JSValueConst *argv) {
        std::tuple<Args...> parsedArgs = convertArgs<Args...>(ctx, argv);
        if constexpr (std::is_void_v<Ret>) {
            std::apply([&](auto &&... unpacked) { fn(obj, unpacked...); }, parsedArgs);
            return JS_UNDEFINED;
        } else {
            Ret r = std::apply([&](auto &&... unpacked) { return fn(obj, unpacked...); }, parsedArgs);
            return JSConverter<Ret>::toJS(ctx, r);
        }
    }

    template<typename... Args>
    static std::tuple<Args...> convertArgs(JSContext *ctx, JSValueConst *argv) {
        return std::make_tuple(JSConverter<Args>::fromJS(ctx, argv[0])...);
    }

    static JSValue methodDispatcher(JSContext *ctx,
    JSValueConst this_val, int argc, JSValueConst *argv, int magic)
    {
        auto *obj = static_cast<Class*>(JS_GetOpaque2(ctx, this_val, s_classId));
        if (!obj) return JS_ThrowTypeError(ctx, "Invalid native object");
        return s_methods[magic].invoker(ctx, obj, argc, argv);
    }

private:
    static inline JSClassID s_classId{0};
    static inline bool s_inited{false};
    static inline std::string s_className{"NativeObject"};
    static inline std::vector<FieldBase> s_fields{};
    static inline std::vector<MethodBase> s_methods{};
};

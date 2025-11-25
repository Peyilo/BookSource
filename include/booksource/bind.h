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
        for (const auto &f : s_fields) {
            JSValue v = f.getter(ctx, instance);
            JS_SetPropertyStr(ctx, obj,f.name.c_str(), v);
        }

        return obj;
    }

    /// 设置类名，仅用于调试/错误信息
    static void setClassName(const std::string &name);

private:
    struct FieldBase {
        std::string name;
        std::function<JSValue(JSContext *, Class *)> getter;
        std::function<void(JSContext *, Class *, JSValueConst)> setter;
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

private:
    static inline JSClassID s_classId{0};
    static inline bool s_inited{false};
    static inline std::string s_className{"NativeObject"};
    static inline std::vector<FieldBase> s_fields{};
};

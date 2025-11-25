#pragma once
#include <quickjs/quickjs.h>
#include <string>
#include <functional>
#include <vector>

template<typename T>
struct JSConverter;

template<typename T>
struct MethodTraits;

template<typename T>
struct FieldTraits;

template<typename T>
class JsBinder;

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

template<typename U>
struct JSConverter<U*> {
    static U* fromJS(JSContext *ctx, JSValueConst v) {
        return static_cast<U*>(JS_GetOpaque2(ctx, v, JsBinder<U>::getClassID()));
    }

    static JSValue toJS(JSContext *ctx, U* ptr) {
        return JsBinder<U>::wrap(ctx, ptr);
    }
};

template<typename U>
struct JSConverter {
    static U fromJS(JSContext *ctx, JSValueConst v) {
        return *static_cast<U*>(JS_GetOpaque2(ctx, v, JsBinder<U>::getClassID()));
    }

    static JSValue toJS(JSContext *ctx, U* ptr) {
        return JsBinder<U>::wrap(ctx, ptr);
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

// 非 const
template<typename C, typename Ret, typename... Args>
struct MethodTraits<Ret (C::*)(Args...)> {
    using Class = C;
    using Return = Ret;
    using ArgsTuple = std::tuple<Args...>;
    static constexpr bool isConst = false;
};

// const 版本
template<typename C, typename Ret, typename... Args>
struct MethodTraits<Ret (C::*)(Args...) const> {
    using Class = C;
    using Return = Ret;
    using ArgsTuple = std::tuple<Args...>;
    static constexpr bool isConst = true;
};



template<typename C, typename FieldType>
struct FieldTraits<FieldType C::*> {
    using Class = C;
    using Type = FieldType;
};

template<typename T>
class JsBinder {
public:
    template<typename FieldPtr>
    static void addField(const std::string &name, FieldPtr ptr) {
        using Traits = FieldTraits<FieldPtr>;
        using FieldType = Traits::Type;

        FieldBase base;
        base.name = name;

        // getter
        base.getter = [ptr](JSContext *ctx, T *obj) -> JSValue {
            return JSConverter<FieldType>::toJS(ctx, obj->*ptr);
        };

        // setter
        base.setter = [ptr](JSContext *ctx, T *obj, JSValueConst jsVal) {
            obj->*ptr = JSConverter<FieldType>::fromJS(ctx, jsVal);
        };

        s_fields.push_back(std::move(base));
    }

    template<typename Method>
    static void addMethod(const std::string &name, Method m) {
        using Traits = MethodTraits<Method>;
        using Return = Traits::Return;
        using ArgsTuple = Traits::ArgsTuple;

        MethodBase base;
        base.name = name;

        base.invoker = [m](JSContext *ctx, T *obj, int argc, JSValueConst *argv) -> JSValue {
            return callMethod<Return>(ctx, obj, m, argv, std::make_index_sequence<std::tuple_size_v<ArgsTuple> >{});
        };

        s_methods.push_back(std::move(base));
    }

    /// 把原生对象包装成 JS 对象（**不负责释放 Class* 的生命周期**）
    static JSValue wrap(JSContext *ctx, T *instance) {
        ensureClassInit(ctx);

        const JSValue obj = JS_NewObjectClass(ctx, s_classId);
        if (JS_IsException(obj)) {
            return obj;
        }

        // 只保存指针，不负责 delete
        JS_SetOpaque(obj, instance);

        return obj;
    }

    /// 设置类名，仅用于调试/错误信息
    static void setClassName(const std::string &name) {
        s_className = name;
    }

    static JSClassID getClassID() {
        return s_classId;
    }

private:
    struct FieldBase {
        std::string name;
        std::function<JSValue(JSContext *, T *)> getter;
        std::function<void(JSContext *, T *, JSValueConst)> setter;
    };

    struct MethodBase {
        std::string name;
        std::function<JSValue(JSContext *, T *, int, JSValueConst *)> invoker;
    };

    // 确保Class已经被注册了
    static void ensureClassInit(JSContext *ctx) {
        JSRuntime *rt = JS_GetRuntime(ctx);
        if (s_inited)
            return;

        JS_NewClassID(&s_classId); // 申请一个新的class_id，保证唯一，用于区分不同的class

        JSClassDef def{};
        def.class_name = s_className.c_str();

        if (JS_NewClass(rt, s_classId, &def) < 0) {
            // 这里抛异常比静默失败要好
            throw std::runtime_error("JS_NewClass failed for " + s_className);
        }

        build(ctx);

        s_inited = true;
    }

    template<typename Return, typename Method, size_t... I>
    static JSValue callMethod(JSContext *ctx, T *obj,
                              Method m,
                              JSValueConst *argv,
                              std::index_sequence<I...>) {
        using Traits = MethodTraits<Method>;
        using ArgsTuple = Traits::ArgsTuple;

        // 参数类型解包
        auto nativeRet = (obj->*m)(
            JSConverter<std::tuple_element_t<I, ArgsTuple> >::fromJS(ctx, argv[I])...
        );

        if constexpr (std::is_same_v<Return, void>) {
            return JS_UNDEFINED;
        } else {
            return JSConverter<Return>::toJS(ctx, nativeRet);
        }
    }


    static JSValue methodDispatcher(JSContext *ctx,
                                    JSValueConst this_val, int argc, JSValueConst *argv, int magic) {
        auto *obj = static_cast<T *>(JS_GetOpaque2(ctx, this_val, s_classId));
        if (!obj) return JS_ThrowTypeError(ctx, "Invalid native object");
        return s_methods[magic].invoker(ctx, obj, argc, argv);
    }

    static void build(JSContext *ctx) {
        const JSValue proto = JS_NewObject(ctx);

        for (int i = 0; i < s_fields.size(); i++) {
            const auto &f = s_fields[i];

            JSAtom atom = JS_NewAtom(ctx, f.name.c_str());

            JS_DefinePropertyGetSet(
                ctx,
                proto,
                atom,
                JS_NewCFunctionMagic(ctx, getterDispatcher, f.name.c_str(), 0, JS_CFUNC_generic_magic, i),
                JS_NewCFunctionMagic(ctx, setterDispatcher, f.name.c_str(), 1, JS_CFUNC_generic_magic, i),
                JS_PROP_ENUMERABLE | JS_PROP_CONFIGURABLE
            );

            JS_FreeAtom(ctx, atom);
        }

        // 方法绑定（无需 magic index 变更）
        for (int i = 0; i < s_methods.size(); i++) {
            JS_SetPropertyStr(
                ctx,
                proto,
                s_methods[i].name.c_str(),
                JS_NewCFunctionMagic(ctx, &methodDispatcher, s_methods[i].name.c_str(),
                                     0, JS_CFUNC_generic_magic, i)
            );
        }

        JS_SetClassProto(ctx, s_classId, proto);
    }

    static JSValue getterDispatcher(JSContext *ctx, JSValueConst this_val,
                                    int argc, JSValueConst *argv, int magic) {
        auto *obj = static_cast<T *>(JS_GetOpaque2(ctx, this_val, s_classId));
        if (!obj) return JS_ThrowTypeError(ctx, "Invalid native object");
        return s_fields[magic].getter(ctx, obj);
    }

    static JSValue setterDispatcher(JSContext *ctx, JSValueConst this_val,
                                    int argc, JSValueConst *argv, int magic) {
        auto *obj = static_cast<T *>(JS_GetOpaque2(ctx, this_val, s_classId));
        if (!obj) return JS_ThrowTypeError(ctx, "Invalid native object");
        s_fields[magic].setter(ctx, obj, argv[0]);
        return JS_UNDEFINED;
    }

private:
    static inline JSClassID s_classId{0};
    static inline bool s_inited{false};
    static inline std::string s_className{"NativeObject"};
    static inline std::vector<FieldBase> s_fields{};
    static inline std::vector<MethodBase> s_methods{};
};


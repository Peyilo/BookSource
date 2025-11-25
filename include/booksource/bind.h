#pragma once
#include <quickjs/quickjs.h>
#include <string>
#include <utility>
#include <type_traits>

// =======================================================
// JS <-> C++ 类型转换器
// =======================================================

template<typename T>
struct JSConverter;

// int
template<> struct JSConverter<int> {
    static int fromJS(JSContext* ctx, JSValueConst v) {
        int32_t out; JS_ToInt32(ctx, &out, v); return out;
    }
    static JSValue toJS(JSContext* ctx, int v) { return JS_NewInt32(ctx, v); }
};

// double
template<> struct JSConverter<double> {
    static double fromJS(JSContext* ctx, JSValueConst v) {
        double out; JS_ToFloat64(ctx, &out, v); return out;
    }
    static JSValue toJS(JSContext* ctx, double v) { return JS_NewFloat64(ctx, v); }
};

// bool
template<> struct JSConverter<bool> {
    static bool fromJS(JSContext* ctx, JSValueConst v) {
        return JS_ToBool(ctx, v) == 1;
    }
    static JSValue toJS(JSContext* ctx, bool v) { return JS_NewBool(ctx, v); }
};

// std::string
template<> struct JSConverter<std::string> {
    static std::string fromJS(JSContext* ctx, JSValueConst v) {
        const char* s = JS_ToCString(ctx, v);
        std::string out = s ? s : "";
        JS_FreeCString(ctx, s);
        return out;
    }
    static JSValue toJS(JSContext* ctx, const std::string& v) {
        return JS_NewString(ctx, v.c_str());
    }
};

// =======================================================
// 🔥 必须补丁：支持 const T& / T& / const T
// =======================================================

template<typename T>
struct JSConverter<const T&> : JSConverter<T> {};

template<typename T>
struct JSConverter<T&> : JSConverter<T> {};

template<typename T>
struct JSConverter<const T> : JSConverter<T> {};


// =======================================================
// 类信息 ClassInfo<T>
// =======================================================

template<typename T>
struct ClassInfo {
    static JSClassID classId;
    static JSValue proto;
};

template<typename T> JSClassID ClassInfo<T>::classId = 0;
template<typename T> JSValue ClassInfo<T>::proto = JS_UNDEFINED;


// =======================================================
// 字段绑定：必须使用静态字段指针表
// =======================================================

template<typename T, typename Field>
struct FieldInfo {
    static Field T::* ptr;
};

template<typename T, typename Field>
Field T::* FieldInfo<T,Field>::ptr = nullptr;


// getter（必须是纯 C 函数）
template<typename T, typename Field>
static JSValue js_field_get(JSContext* ctx,
                            JSValueConst this_val,
                            int argc,
                            JSValueConst* argv)
{
    T* obj = static_cast<T*>(JS_GetOpaque2(ctx, this_val, ClassInfo<T>::classId));
    return JSConverter<Field>::toJS(ctx, obj->*FieldInfo<T,Field>::ptr);
}


// setter
template<typename T, typename Field>
static JSValue js_field_set(JSContext* ctx,
                            JSValueConst this_val,
                            int argc,
                            JSValueConst* argv)
{
    T* obj = static_cast<T*>(JS_GetOpaque2(ctx, this_val, ClassInfo<T>::classId));
    obj->*FieldInfo<T,Field>::ptr = JSConverter<Field>::fromJS(ctx, argv[0]);
    return JS_UNDEFINED;
}


template<typename T, typename Field>
void bind_field(JSContext* ctx, JSValue proto, const char* name, Field T::* field)
{
    FieldInfo<T,Field>::ptr = field;

    JS_DefinePropertyGetSet(
        ctx,
        proto,
        JS_NewAtom(ctx, name),
        JS_NewCFunction(ctx, js_field_get<T,Field>, name, 0),
        JS_NewCFunction(ctx, js_field_set<T,Field>, name, 1),
        0
    );
}


// =======================================================
// 方法绑定：必须使用静态方法指针表
// =======================================================

template<typename T, typename Ret, typename... Args>
struct MethodInfo {
    static Ret(T::* ptr)(Args...);
};

template<typename T, typename Ret, typename... Args>
Ret(T::* MethodInfo<T,Ret,Args...>::ptr)(Args...) = nullptr;


template<typename T, typename Ret, typename... Args, size_t... I>
JSValue call_method_impl(
    JSContext* ctx,
    T* obj,
    JSValueConst* argv,
    std::index_sequence<I...>)
{
    if constexpr (std::is_void_v<Ret>) {
        (obj->*MethodInfo<T,Ret,Args...>::ptr)(
            JSConverter<Args>::fromJS(ctx, argv[I])...
        );
        return JS_UNDEFINED;
    } else {
        Ret r = (obj->*MethodInfo<T,Ret,Args...>::ptr)(
            JSConverter<Args>::fromJS(ctx, argv[I])...
        );
        return JSConverter<Ret>::toJS(ctx, r);
    }
}

// 包装器：必须为静态函数
template<typename T, typename Ret, typename... Args>
static JSValue js_method_call(
    JSContext* ctx,
    JSValueConst this_val,
    int argc,
    JSValueConst* argv)
{
    T* obj = static_cast<T*>(JS_GetOpaque2(ctx, this_val, ClassInfo<T>::classId));
    return call_method_impl<T, Ret, Args...>(
        ctx, obj, argv, std::index_sequence_for<Args...>{}
    );
}

template<typename T, typename Ret, typename... Args>
void bind_method(JSContext* ctx, JSValue proto,
                 const char* name,
                 Ret(T::*func)(Args...))
{
    MethodInfo<T,Ret,Args...>::ptr = func;

    JS_SetPropertyStr(
        ctx,
        proto,
        name,
        JS_NewCFunction(ctx,
            js_method_call<T,Ret,Args...>,
            name,
            sizeof...(Args))
    );
}


// =======================================================
// 绑定入口 DSL
// =======================================================

struct JSCBinding {
    JSContext* ctx;

    explicit JSCBinding(JSContext* c) : ctx(c) {}

    template<typename T>
    static JSCBinding class_(JSContext* ctx, const char* name)
    {
        JS_NewClassID(&ClassInfo<T>::classId);

        JSClassDef def{};
        def.class_name = name;
        def.finalizer = [](JSRuntime* rt, JSValue val){
            T* obj = static_cast<T*>(JS_GetOpaque(val, ClassInfo<T>::classId));
            delete obj;
        };

        JS_NewClass(JS_GetRuntime(ctx), ClassInfo<T>::classId, &def);

        ClassInfo<T>::proto = JS_NewObject(ctx);
        JS_SetClassProto(ctx, ClassInfo<T>::classId, ClassInfo<T>::proto);

        return JSCBinding(ctx);
    }

    template<typename T, typename Field>
    JSCBinding& field(const char* name, Field T::* field)
    {
        bind_field<T,Field>(ctx, ClassInfo<T>::proto, name, field);
        return *this;
    }

    template<typename T, typename Ret, typename... Args>
    JSCBinding& method(const char* name, Ret(T::*func)(Args...))
    {
        bind_method<T,Ret,Args...>(ctx, ClassInfo<T>::proto, name, func);
        return *this;
    }
};

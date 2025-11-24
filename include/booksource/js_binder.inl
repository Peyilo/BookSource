#pragma once

#include <cassert>
#include "js_binder.h"

template<typename T>
void JsBinder<T>::addField(const std::string &name, FieldGetter getter) {
    s_fields.push_back(Field{name, std::move(getter)});
}

template<typename T>
void JsBinder<T>::addMethod(const std::string &name, MethodFunc func) {
    s_methods.push_back(Method{name, std::move(func)});
}

template<typename T>
void JsBinder<T>::setClassName(const std::string &name) {
    s_className = name;
}

template<typename T>
void JsBinder<T>::ensureClassInit(JSRuntime *rt) {
    if (s_inited)
        return;

    JS_NewClassID(&s_classId);

    JSClassDef def{};
    def.class_name = s_className.c_str();
    def.finalizer  = &JsBinder<T>::finalizer;

    if (JS_NewClass(rt, s_classId, &def) < 0) {
        // 这里抛异常比静默失败要好
        throw std::runtime_error("JS_NewClass failed for " + s_className);
    }

    s_inited = true;
}

template<typename T>
JSValue JsBinder<T>::wrap(JSContext *ctx, T *instance) {
    JSRuntime *rt = JS_GetRuntime(ctx);
    ensureClassInit(rt);

    JSValue obj = JS_NewObjectClass(ctx, s_classId);
    if (JS_IsException(obj)) {
        return obj;
    }

    // 只保存指针，不负责 delete
    JS_SetOpaque(obj, instance);

    // 设置字段（快照）
    for (const auto &f : s_fields) {
        std::string val = f.getter(instance);
        JS_SetPropertyStr(
            ctx,
            obj,
            f.name.c_str(),
            JS_NewStringLen(ctx, val.c_str(), val.size())
        );
    }

    // 注册方法
    for (int i = 0; i < static_cast<int>(s_methods.size()); ++i) {
        const auto &m = s_methods[i];
        JSValue fn = JS_NewCFunctionMagic(
            ctx,
            &JsBinder<T>::methodDispatcher,
            m.name.c_str(),
            0,                          // nargs: 让 JS 自由传参
            JS_CFUNC_generic_magic,
            i                           // magic = 方法索引
        );
        JS_SetPropertyStr(ctx, obj, m.name.c_str(), fn);
    }

    return obj;
}

template<typename T>
JSValue JsBinder<T>::methodDispatcher(JSContext *ctx,
                                      JSValueConst this_val,
                                      int argc,
                                      JSValueConst *argv,
                                      int magic) {
    if (magic < 0 || magic >= static_cast<int>(s_methods.size())) {
        return JS_ThrowInternalError(ctx, "invalid method magic");
    }

    auto *ptr = static_cast<T *>(JS_GetOpaque(this_val, s_classId));
    if (!ptr) {
        return JS_ThrowTypeError(ctx, "this is not a %s", s_className.c_str());
    }

    const auto &method = s_methods[magic];
    if (!method.func) {
        return JS_ThrowInternalError(ctx, "method not bound");
    }

    return method.func(ctx, ptr, argc, argv);
}

template<typename T>
void JsBinder<T>::finalizer(JSRuntime *rt, JSValue val) {
    (void)rt;
    auto *ptr = static_cast<T *>(JS_GetOpaque(val, s_classId));
    // 默认不 delete，由外部管理 T* 生命周期
    // 如果你希望在 GC 时 delete，对单个 T 特化这里即可。
    (void)ptr;
}
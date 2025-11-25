#pragma once

#include <quickjs/quickjs.h>
#include <functional>
#include <string>
#include <vector>

template<typename Class>
class JsBinder {
public:
    template<typename FieldType>
    using FieldGetter = std::function<FieldType (Class *instance)>;

    template<typename FieldType>
    using FieldSetter = std::function<void (Class *instance, FieldType value)>;

    template<typename FieldType>
    struct Field {
        std::string name;
        FieldGetter<FieldType> getter;
        FieldSetter<FieldType> setter;
    };

    /// 注册一个字段（在 wrap() 时快照为字符串属性）
    /// 注册字段：getter + setter
    template<typename FieldType>
    static void addField(const std::string &name,
                         FieldGetter<FieldType> getter,
                         FieldSetter<FieldType> setter)
    {
        // 将字段记录为 type-erased 形式
        FieldBase base;
        base.name = name;

        // getter 封装
        base.getter = [getter](JSContext* ctx, Class* obj) -> JSValue {
            FieldType v = getter(obj);
            return toJs(ctx, v);
        };

        // setter 封装
        if (setter) {
            base.setter = [setter](JSContext* ctx, Class* obj, JSValueConst val) {
                FieldType native;
                fromJs(ctx, val, native);
                setter(obj, native);
            };
        }

        s_fields.push_back(std::move(base));
    }

    /// 把原生对象包装成 JS 对象（**不负责释放 Class* 的生命周期**）
    static JSValue wrap(JSContext *ctx, Class *instance);

    /// 设置类名，仅用于调试/错误信息
    static void setClassName(const std::string &name);

private:
    static void ensureClassInit(JSRuntime *rt);

    static JSValue methodDispatcher(JSContext *ctx,
                                    JSValueConst this_val,
                                    int argc,
                                    JSValueConst *argv,
                                    int magic);

    static void finalizer(JSRuntime *rt, JSValue val);

    // ========== C++ <-> JS 类型转换接口（需要你实现） ==========
    template<typename FieldType>
    static JSValue toJs(JSContext* ctx, const FieldType& v);

    template<typename FieldType>
    static void fromJs(JSContext* ctx, JSValueConst js, FieldType& out);

private:
    struct FieldBase {
        std::string name;
        std::function<JSValue(JSContext*, Class*)> getter;
        std::function<void(JSContext*, Class*, JSValueConst)> setter;
    };

    static inline JSClassID s_classId{0};
    static inline bool s_inited{false};
    static inline std::string s_className{"NativeObject"};
    static inline std::vector<FieldBase> s_fields{};
};

#include "js_binder.inl"

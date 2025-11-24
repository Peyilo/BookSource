#include <iostream>
#include <quickjs.h>

int main() {
    // 创建运行时（VM）
    JSRuntime *rt = JS_NewRuntime();
    if (!rt) {
        printf("Failed to create JSRuntime\n");
        return 1;
    }

    // 创建上下文（执行环境）
    JSContext *ctx = JS_NewContext(rt);
    if (!ctx) {
        printf("Failed to create JSContext\n");
        JS_FreeRuntime(rt);
        return 1;
    }

    const char *script = "1 + 2 * 3";

    // 执行脚本
    JSValue result = JS_Eval(ctx, script, strlen(script), "<eval>", JS_EVAL_TYPE_GLOBAL);

    if (JS_IsException(result)) {
        printf("JavaScript Error!\n");
        JSValue err = JS_GetException(ctx);
        const char *errstr = JS_ToCString(ctx, err);
        printf("Error: %s\n", errstr);
        JS_FreeCString(ctx, errstr);
        JS_FreeValue(ctx, err);
    } else {
        // 转换为 int
        int32_t value = 0;
        JS_ToInt32(ctx, &value, result);
        printf("JS result = %d\n", value);
    }

    JS_FreeValue(ctx, result);
    JS_FreeContext(ctx);
    JS_FreeRuntime(rt);

    return 0;
}

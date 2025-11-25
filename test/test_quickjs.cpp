//
// Created by Peyilo on 2025/11/25.
//
#include <quickjs/quickjs.h>
#include <iostream>
#include <booksource/engine.h>
#include <fstream>
#include <sstream>
#include <string>

/**
 * @param filename 文件路径
 * @return 读取该文件的内容，以字符串形式返回
 */
std::string load_js_file(const std::string& filename)
{
    std::ifstream file(filename, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        return {};
    }

    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

/**
 * 读取js文件，并在给定的引擎中执行
 * @param engine Quick Js引擎
 */
void test_complex_js(QuickJsEngine &engine) {
#ifdef WIN32
    const std::string filename = R"(D:\Code\Clion\booksource\test\complex_test.js)";
#else
    const std::string filename = R"()";
#endif
    const std::string script = load_js_file(filename);
    if (script.empty()) {
        fprintf(stderr, "Failed to load JS file: %s\n", filename.c_str());
        return;
    }
    const auto result = engine.eval(script);
    std::cout << "[Result] " << result << std::endl;
}

// 供js中调用print
static JSValue js_print(
    JSContext* ctx,
    JSValueConst _,
    const int argc,
    JSValueConst* argv) {
    for (int i = 0; i < argc; i++) {
        const char* str = JS_ToCString(ctx, argv[i]);
        if (!str)
            return JS_EXCEPTION;

        std::cout << str;
        JS_FreeCString(ctx, str);

        if (i + 1 < argc)
            std::cout << " ";
    }

    std::cout << std::endl;
    return JS_UNDEFINED;
}

// 绑定print函数
void add_print(JSContext* ctx) {
    const JSValue global = JS_GetGlobalObject(ctx);

    JS_SetPropertyStr(
        ctx,
        global,
        "print",
        JS_NewCFunction(ctx, js_print, "print", 1)
    );

    JS_FreeValue(ctx, global);
}

int main() {
    QuickJsEngine engine;
    add_print(engine.getContext());
    test_complex_js(engine);

    return 0;
}
//
// Created by Peyilo on 2025/11/25.
//
#include <iostream>
#include <booksource/engine.h>
#include <fstream>
#include <sstream>
#include <string>
#include "test_utils.h"

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
    const std::string filename = std::string(CMAKE_CURRENT_SOURCE_DIR) + "/resource/complex_test.js";
    const std::string script = load_js_file(filename);
    if (script.empty()) {
        fprintf(stderr, "Failed to load JS file: %s\n", filename.c_str());
        return;
    }
    const auto result = engine.eval(script);
    std::cout << "[Result] " << result << std::endl;
}

int main() {
    QuickJsEngine engine;
    engine.addPrintFunc("print");
    test_complex_js(engine);
    engine.reset();
    return 0;
}
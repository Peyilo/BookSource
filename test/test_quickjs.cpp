//
// Created by Peyilo on 2025/11/25.
//
#include <iostream>
#include <booksource/engine.h>
#include <fstream>
#include <string>
#include "test_utils.h"

/**
 * 读取js文件，并在给定的引擎中执行
 * @param engine Quick Js引擎
 */
void test_complex_js(QuickJsEngine &engine) {
    const std::string script = getResourceText("complex_test.js");
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
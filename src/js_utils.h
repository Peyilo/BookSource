//
// Created by Peyilo on 2025/11/24.
//

#pragma once
#include <string>

// 执行 JS 并返回字符串
// 任何返回值都会以 string 形式返回
// 如果 JS 抛异常，会抛出 std::runtime_error
std::string run_simple_js(const std::string& code);

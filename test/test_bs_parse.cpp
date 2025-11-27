//
// Created by 夏莳 on 2025/11/26.
//

#include <iostream>
#include <booksource/booksource_parser.h>
#include "test_utils.h"

int main() {
    const auto bss1 = BookSourceParser::parseBookSourceList(getResourceText("bs1.json"));
    std::cout << "bss1 size: " << bss1.size() << std::endl;
    const auto bss2 = BookSourceParser::parseBookSourceList(getResourceText("bs2.json"));
    std::cout << "bss2 size: " << bss2.size() << std::endl;
    return 0;
}
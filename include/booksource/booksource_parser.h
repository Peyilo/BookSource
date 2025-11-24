#pragma once

#include <booksource/booksource.h>
#include <vector>

class BookSourceParser {
public:
    static BookSource parseBookSource(const std::string& jsonStr);

    static std::vector<BookSource> parseBookSourceList(const std::string& jsonStr);
};

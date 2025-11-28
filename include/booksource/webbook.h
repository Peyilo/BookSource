#pragma once

#include <vector>
#include <string>
#include <chrono>
#include <functional>
#include <booksource/booksource.h>
#include <unordered_map>

using LocalDate = std::chrono::year_month_day;

inline LocalDate generateLocalDate(
    const int year, const unsigned month, const unsigned day
) {
    const auto date = LocalDate{
        std::chrono::year{year},
        std::chrono::month{month},
        std::chrono::day{day}
    };
    if (!date.ok()) {
        throw std::invalid_argument("Invalid date");
    }
    return date;
}

inline LocalDate defaultLocalDate() {
    return generateLocalDate(1970, 1, 1);
}

// 获取当前的时间戳：毫秒ms
inline long currentTimeMillis() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(
        system_clock::now().time_since_epoch()
    ).count();
}


namespace BookType {

    constexpr int text        = 0b1000;         // 8
    constexpr int updateError = 0b10000;        // 16
    constexpr int audio       = 0b100000;       // 32
    constexpr int image       = 0b1000000;      // 64
    constexpr int webFile     = 0b10000000;     // 128
    constexpr int local       = 0b100000000;    // 256
    constexpr int archive     = 0b1000000000;   // 512
    constexpr int notShelf    = 0b10000000000;  // 1024

    constexpr int allBookType = text | image | audio | webFile;
    constexpr int allBookTypeLocal = allBookType | local;

    // 字符串常量
    constexpr auto localTag = "loc_book";
    constexpr auto webDavTag = "webDav::";

}

struct ReadConfig {
    bool reverseToc = false;
    int pageAnim = -1;
    bool reSegment = false;
    std::string imageStyle;
    bool useReplaceRule = false;
    long delTag = 0L;
    std::string ttsEngine;
    bool splitLongChapter = true;
    bool readSimulating = false;
    LocalDate startDate = defaultLocalDate();
    int startChapter = -1;
    int dailyChapters = 3;
};

struct Book {
    // 详情页Url(本地书源存储完整文件路径)
    std::string bookUrl;
    // 目录页Url (toc=table of Contents)
    std::string tocUrl;
    // 书源URL(默认BookType.localTag)
    std::string origin = BookType::localTag;   // 需要你自己替换 BookType.localTag 对应字符串
    // 书源名称 or 本地书籍文件名
    std::string originName;
    // 书籍名称(书源获取)
    std::string name;
    // 作者名称(书源获取)
    std::string author;
    // 分类信息(书源获取)
    std::string kind;
    // 分类信息(用户修改)
    std::string customTag;
    // 封面Url(书源获取)
    std::string coverUrl;
    // 封面Url(用户修改)
    std::string customCoverUrl;
    // 简介内容(书源获取)
    std::string intro;
    // 简介内容(用户修改)
    std::string customIntro;
    // 自定义字符集名称(仅适用于本地书籍)
    std::string charset;
    // 类型
    int type = 0; // BookType.text → 你需要自己定义枚举
    // 自定义分组索引号
    long group = 0;
    // 最新章节标题
    std::string latestChapterTitle;
    // 最新章节标题更新时间
    long latestChapterTime = currentTimeMillis();
    // 最近一次更新书籍信息的时间
    long lastCheckTime = currentTimeMillis();
    // 最近一次发现新章节的数量
    int lastCheckCount = 0;
    // 书籍目录总数
    int totalChapterNum = 0;
    // 当前章节名称
    std::string durChapterTitle;
    // 当前章节索引
    int durChapterIndex = 0;
    // 当前阅读的进度(首行字符的索引位置)
    int durChapterPos = 0;
    // 最近一次阅读书籍的时间
    long durChapterTime = currentTimeMillis();
    // 字数
    std::string wordCount;
    // 刷新书架时更新书籍信息
    bool canUpdate = true;
    // 手动排序
    int order = 0;
    // 书源排序
    int originOrder = 0;
    // 自定义书籍变量信息
    std::string variable;
    // 阅读设置
    ReadConfig readConfig;    // Kotlin 是 nullable，但你不想 optional → 内嵌结构体
    // 同步时间
    long syncTime = 0;

    std::string infoHtml;
    std::string tocHtml;
    std::vector<std::string> downloadUrls;
};

struct SearchBook {
    std::string bookUrl;
    // 书源
    std::string origin;
    std::string originName;
    // BookType
    int type = BookType::text;
    std::string name;
    std::string author;
    std::string kind;
    std::string coverUrl;
    std::string intro;
    std::string wordCount;
    std::string latestChapterTitle;

    /** 目录页Url (toc=table of Contents) */
    std::string tocUrl;

    long time = currentTimeMillis();
    std::string variable;
    int originOrder = 0;
    std::string chapterWordCountText;
    int chapterWordCount = -1;
    int respondTime = -1;

    std::string infoHtml;
    std::string tocHtml;
};

class RuleDataInterface {

public:
    std::unordered_map<std::string, std::string> variableMap;

    virtual ~RuleDataInterface() = default;

    // 需要子类实现：用于保存/移除大变量（例如存入文件）
    virtual void putBigVariable(const std::string& key, const std::string* value) = 0;

    // 需要子类实现：获取大变量
    virtual std::string getBigVariable(const std::string& key) const = 0;

    // Kotlin 中的默认实现：putVariable
    bool putVariable(const std::string& key, const std::string& value) {
        const bool keyExist = variableMap.contains(key);

        // value == null → C++ 无法用 null 表示字符串 → 这里用 empty string 特殊标记
        if (value.empty()) {
            variableMap.erase(key);
            putBigVariable(key, nullptr);
            return keyExist;
        }

        if (value.size() < 10000) {
            putBigVariable(key, nullptr);
            variableMap[key] = value;
            return true;
        }

        variableMap.erase(key);
        putBigVariable(key, &value);
        return keyExist;
    }

    // Kotlin: fun getVariable(...)：优先小变量，再大变量，否则 ""
    std::string getVariable(const std::string& key) const {
        if (const auto it = variableMap.find(key); it != variableMap.end()) {
            return it->second;
        }
        if (auto big = getBigVariable(key); !big.empty()) return big;
        return "";
    }
};

class RuleData {
public:

};

namespace WebBook {

    using BookFilter = std::function<bool(std::string name, std::string author)>;

    using BreakCondition = std::function<bool(int size)>;

    std::vector<SearchBook> searchBook(
        BookSource bookSource,
        std::string key,
        int page,
        BookFilter filter,
        BreakCondition shouldBreak
    ) {
        std::string &searchUrl = bookSource.searchUrl;
        if (searchUrl.empty()) {
            // throw NoStackTraceException("搜索url不能为空")
        }
        // TODO
        return std::vector<SearchBook>{};
    }

}
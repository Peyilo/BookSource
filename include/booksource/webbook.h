#pragma once

#include <vector>
#include <string>
#include <chrono>

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

struct ReadConfig {
    bool reverseToc = false;
    int pageAnim = -1;
    bool reSegment = false;
    std::string imageStyle = "";
    bool useReplaceRule = false;
    long delTag = 0L;
    std::string ttsEngine = "";
    bool splitLongChapter = true;
    bool readSimulating = false;
    LocalDate startDate = defaultLocalDate();
    int startChapter = -1;
    int dailyChapters = 3;
};

struct Book {
    // 详情页Url(本地书源存储完整文件路径)
    std::string bookUrl = "";
    // 目录页Url
    std::string tocUrl = "";
    // 书源URL(默认BookType.localTag)
    std::string origin = "local";   // 需要你自己替换 BookType.localTag 对应字符串
    // 书源名称 or 本地书籍文件名
    std::string originName = "";
    // 书籍名称(书源获取)
    std::string name = "";
    // 作者名称(书源获取)
    std::string author = "";
    // 分类信息(书源获取)
    std::string kind = "";
    // 分类信息(用户修改)
    std::string customTag = "";
    // 封面Url(书源获取)
    std::string coverUrl = "";
    // 封面Url(用户修改)
    std::string customCoverUrl = "";
    // 简介内容(书源获取)
    std::string intro = "";
    // 简介内容(用户修改)
    std::string customIntro = "";
    // 自定义字符集名称(仅适用于本地书籍)
    std::string charset = "";
    // 类型
    int type = 0; // BookType.text → 你需要自己定义枚举

    // 自定义分组索引号
    long group = 0;

    // 最新章节标题
    std::string latestChapterTitle = "";

    // 最新章节标题更新时间
    long latestChapterTime = currentTimeMillis();

    // 最近一次更新书籍信息的时间
    long lastCheckTime = currentTimeMillis();

    // 最近一次发现新章节的数量
    int lastCheckCount = 0;

    // 书籍目录总数
    int totalChapterNum = 0;

    // 当前章节名称
    std::string durChapterTitle = "";

    // 当前章节索引
    int durChapterIndex = 0;

    // 当前阅读的进度(首行字符的索引位置)
    int durChapterPos = 0;

    // 最近一次阅读书籍的时间
    long durChapterTime = currentTimeMillis();

    // 字数
    std::string wordCount = "";

    // 刷新书架时更新书籍信息
    bool canUpdate = true;

    // 手动排序
    int order = 0;

    // 书源排序
    int originOrder = 0;

    // 自定义书籍变量信息
    std::string variable = "";

    // 阅读设置
    ReadConfig readConfig;    // Kotlin 是 nullable，但你不想 optional → 内嵌结构体

    // 同步时间
    long syncTime = 0;
};
#pragma once

#include <string>

struct BookInfoRule {
    std::string init;
    std::string name;
    std::string author;
    std::string intro;
    std::string kind;
    std::string lastChapter;
    std::string updateTime;
    std::string coverUrl;
    std::string tocUrl;
    std::string wordCount;
    std::string canReName;
    std::string downloadUrls;
};

struct BookListRule {
    std::string bookList;
    std::string name;
    std::string author;
    std::string intro;
    std::string kind;
    std::string lastChapter;
    std::string updateTime;
    std::string bookUrl;
    std::string coverUrl;
    std::string wordCount;
};

struct ContentRule {
    std::string content;
    std::string title;            // 有些网站只能在正文中获取标题
    std::string nextContentUrl;
    std::string webJs;
    std::string sourceRegex;
    std::string replaceRegex;     // 替换规则
    std::string imageStyle;       // 默认大小居中, FULL最大宽度
    std::string imageDecode;      // 图片bytes二次解密js
    std::string payAction;        // 购买操作, js 或者包含 {{js}} 的 url
};

struct ExploreRule: BookListRule {};

struct ReviewRule {
    std::string reviewUrl;        // 段评URL
    std::string avatarRule;       // 段评发布者头像
    std::string contentRule;      // 段评内容
    std::string postTimeRule;     // 段评发布时间
    std::string reviewQuoteUrl;   // 获取段评回复URL

    std::string voteUpUrl;        // 点赞URL
    std::string voteDownUrl;      // 点踩URL
    std::string postReviewUrl;    // 发送回复URL
    std::string postQuoteUrl;     // 发送回复段评URL
    std::string deleteUrl;        // 删除段评URL
};

struct SearchRule: BookListRule {
    std::string checkKeyWord;     // 校验关键字
};

struct TocRule {
    std::string preUpdateJs;
    std::string chapterList;
    std::string chapterName;
    std::string chapterUrl;
    std::string formatJs;
    std::string isVolume;
    std::string isVip;
    std::string isPay;
    std::string updateTime;
    std::string nextTocUrl;
};

struct BookSource {
    // 基本信息
    std::string bookSourceUrl;        // 地址
    std::string bookSourceName;       // 名称
    std::string bookSourceGroup;      // 分组
    int bookSourceType = 0;           // 类型

    std::string bookUrlPattern;       // 详情页 URL 正则
    int customOrder = 0;              // 手动排序编号
    bool enabled = true;              // 是否启用
    bool enabledExplore = true;       // 是否启用发现

    // 网络配置
    std::string jsLib;                // js库
    bool enabledCookieJar = true;     // 自动 cookie jar
    std::string concurrentRate;       // 并发率
    std::string header;               // 请求头

    // 登录
    std::string loginUrl;
    std::string loginUi;
    std::string loginCheckJs;

    // 封面解密
    std::string coverDecodeJs;

    // 注释
    std::string bookSourceComment;
    std::string variableComment;

    // 排序相关
    int64_t lastUpdateTime = 0;       // 最后更新时间
    int64_t respondTime = 180000;     // 响应时间 ms
    int weight = 0;                   // 智能排序权重

    // 发现
    std::string exploreUrl;
    std::string exploreScreen;
    ExploreRule ruleExplore;

    // 搜索
    std::string searchUrl;
    SearchRule ruleSearch;

    // 书籍详情
    BookInfoRule ruleBookInfo;

    // 目录
    TocRule ruleToc;

    // 正文
    ContentRule ruleContent;

    // 段评
    ReviewRule ruleReview;
};

#include <booksource/booksource_parser.h>
#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;

static void parse(const json &j, BookInfoRule &o) {
    o.init = j.value("init", "");
    o.name = j.value("name", "");
    o.author = j.value("author", "");
    o.intro = j.value("intro", "");
    o.kind = j.value("kind", "");
    o.lastChapter = j.value("lastChapter", "");
    o.updateTime = j.value("updateTime", "");
    o.coverUrl = j.value("coverUrl", "");
    o.tocUrl = j.value("tocUrl", "");
    o.wordCount = j.value("wordCount", "");
    o.canReName = j.value("canReName", "");
    o.downloadUrls = j.value("downloadUrls", "");
}

static void parse(const json &j, BookListRule &o) {
    o.bookList = j.value("bookList", "");
    o.name = j.value("name", "");
    o.author = j.value("author", "");
    o.intro = j.value("intro", "");
    o.kind = j.value("kind", "");
    o.lastChapter = j.value("lastChapter", "");
    o.updateTime = j.value("updateTime", "");
    o.bookUrl = j.value("bookUrl", "");
    o.coverUrl = j.value("coverUrl", "");
    o.wordCount = j.value("wordCount", "");
}

static void parse(const json &j, ExploreRule &o) {
    parse(j, static_cast<BookListRule &>(o));
}

static void parse(const json &j, SearchRule &o) {
    parse(j, static_cast<BookListRule &>(o));
    o.checkKeyWord = j.value("checkKeyWord", "");
}

static void parse(const json &j, ContentRule &o) {
    o.content = j.value("content", "");
    o.title = j.value("title", "");
    o.nextContentUrl = j.value("nextContentUrl", "");
    o.webJs = j.value("webJs", "");
    o.sourceRegex = j.value("sourceRegex", "");
    o.replaceRegex = j.value("replaceRegex", "");
    o.imageStyle = j.value("imageStyle", "");
    o.imageDecode = j.value("imageDecode", "");
    o.payAction = j.value("payAction", "");
}

static void parse(const json &j, ReviewRule &o) {
    o.reviewUrl = j.value("reviewUrl", "");
    o.avatarRule = j.value("avatarRule", "");
    o.contentRule = j.value("contentRule", "");
    o.postTimeRule = j.value("postTimeRule", "");
    o.reviewQuoteUrl = j.value("reviewQuoteUrl", "");

    o.voteUpUrl = j.value("voteUpUrl", "");
    o.voteDownUrl = j.value("voteDownUrl", "");
    o.postReviewUrl = j.value("postReviewUrl", "");
    o.postQuoteUrl = j.value("postQuoteUrl", "");
    o.deleteUrl = j.value("deleteUrl", "");
}

static void parse(const json &j, TocRule &o) {
    o.preUpdateJs = j.value("preUpdateJs", "");
    o.chapterList = j.value("chapterList", "");
    o.chapterName = j.value("chapterName", "");
    o.chapterUrl = j.value("chapterUrl", "");
    o.formatJs = j.value("formatJs", "");
    o.isVolume = j.value("isVolume", "");
    o.isVip = j.value("isVip", "");
    o.isPay = j.value("isPay", "");
    o.updateTime = j.value("updateTime", "");
    o.nextTocUrl = j.value("nextTocUrl", "");
}

static void parse(const json &j, BookSource &o) {
    o.bookSourceUrl = j.value("bookSourceUrl", "");
    o.bookSourceName = j.value("bookSourceName", "");
    o.bookSourceGroup = j.value("bookSourceGroup", "");

    o.bookSourceType = j.value("bookSourceType", 0);
    o.bookUrlPattern = j.value("bookUrlPattern", "");
    o.customOrder = j.value("customOrder", 0);
    o.enabled = j.value("enabled", true);
    o.enabledExplore = j.value("enabledExplore", true);

    o.jsLib = j.value("jsLib", "");
    o.enabledCookieJar = j.value("enabledCookieJar", true);
    o.concurrentRate = j.value("concurrentRate", "");
    o.header = j.value("header", "");

    o.loginUrl = j.value("loginUrl", "");
    o.loginUi = j.value("loginUi", "");
    o.loginCheckJs = j.value("loginCheckJs", "");

    o.coverDecodeJs = j.value("coverDecodeJs", "");

    o.bookSourceComment = j.value("bookSourceComment", "");
    o.variableComment = j.value("variableComment", "");

    o.lastUpdateTime = j.value("lastUpdateTime", 0LL);
    o.respondTime = j.value("respondTime", 180000LL);
    o.weight = j.value("weight", 0);

    o.exploreUrl = j.value("exploreUrl", "");
    o.exploreScreen = j.value("exploreScreen", "");

    o.searchUrl = j.value("searchUrl", "");

    if (j.contains("ruleExplore"))
        parse(j.at("ruleExplore"), o.ruleExplore);

    if (j.contains("ruleSearch"))
        parse(j.at("ruleSearch"), o.ruleSearch);

    if (j.contains("ruleBookInfo"))
        parse(j.at("ruleBookInfo"), o.ruleBookInfo);

    if (j.contains("ruleToc"))
        parse(j.at("ruleToc"), o.ruleToc);

    if (j.contains("ruleContent"))
        parse(j.at("ruleContent"), o.ruleContent);

    if (j.contains("ruleReview"))
        parse(j.at("ruleReview"), o.ruleReview);
}

BookSource BookSourceParser::parseBookSource(const std::string& jsonStr) {
    const json j = json::parse(jsonStr);
    BookSource o;
    parse(j, o);
    return o;
}

std::vector<BookSource> BookSourceParser::parseBookSourceList(const std::string& jsonStr) {
    const json arr = json::parse(jsonStr);
    std::vector<BookSource> list;

    if (!arr.is_array()) {
        std::cerr << "parseBookSourceList 失败：JSON 不是数组\n";
        return list;
    }

    list.reserve(arr.size());           // 预留空间

    for (const auto& item : arr) {
        BookSource o;
        parse(item, o);
        list.push_back(std::move(o));
    }

    return list;
}
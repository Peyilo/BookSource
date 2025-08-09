package org.peyilo.booksource.entities

import org.peyilo.booksource.entities.rule.*

data class BookSource(
    var bookSourceUrl: String = "",
    var bookSourceName: String = "",
    var bookSourceGroup: String? = null,
    var bookSourceType: Int = 0,                        // 类型，0 文本，1 音频, 2 图片, 3 文件（指的是类似知轩藏书只提供下载的网站）

    var bookUrlPattern: String? = null,                 // 详情页url正则

    var customOrder: Int = 0,                           // 手动排序编号

    var enabled: Boolean = true,                        // 是否启用

    var enabledExplore: Boolean = true,                 // 启用发现

    var enabledReview: Boolean? = false,                // 启用段评

    var enabledCookieJar: Boolean? = true,              // 启用okhttp CookieJAr 自动保存每次请求的cookie

    var concurrentRate: String? = null,                 // 并发率

    var header: String? = null,                         // 请求头

    var loginUrl: String? = null,                       // 登录地址

    var loginUi: String? = null,                        // 登录UI

    var loginCheckJs: String? = null,                   // 登录检测js

    var coverDecodeJs: String? = null,                  // 封面解密js

    var bookSourceComment: String? = null,              // 注释

    var variableComment: String? = null,                // 自定义变量说明

    var lastUpdateTime: Long = 0,                       // 最后更新时间，用于排序

    var respondTime: Long = 180000L,                    // 响应时间，用于排序

    var weight: Int = 0,                                // 智能排序的权重

    var exploreUrl: String? = null,                     // 发现url

    var ruleExplore: ExploreRule? = null,               // 发现规则

    var searchUrl: String? = null,                      // 搜索url

    var ruleSearch: SearchRule? = null,                 // 搜索规则

    var ruleBookInfo: BookInfoRule? = null,             // 书籍信息页规则

    var ruleToc: TocRule? = null,                       // 目录页规则

    var ruleContent: ContentRule? = null,               // 正文页规则

    var ruleReview: ReviewRule? = null                  // 段评规则
)

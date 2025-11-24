#include <booksource/engine.h>
#include <iostream>

void test_reset(QuickJsEngine &engine) {
    std::cout << engine.eval("var x = 10; x;") << std::endl; // 10
    std::cout << engine.eval("x = x + 5; x;") << std::endl; // 15

    std::cout << "----- reset engine -----" << std::endl;
    engine.reset(); // 清空状态

    try {
        std::cout << engine.eval("x;") << std::endl; // 这里会报错，因为变量 x 已不存在
    } catch (const std::exception &ex) {
        std::cout << "Error: " << ex.what() << std::endl;
    }

    std::cout << engine.eval("var x = 100; x;") << std::endl; // 100
}

void test_bind(const QuickJsEngine &engine) {
    engine.addBinding("page", "https://google.com");
    engine.addBinding("book", "Lord of the Rings");
    engine.addBinding("key", "abcd1234");

    const std::string result = engine.eval(R"(page + " / " + book + " / " + key)");
    std::cout << result << std::endl;
}

int main() {
    QuickJsEngine engine;
    // test_reset(engine);
    // test_bind(engine);

    const auto script =
    "let sort = [];\n"
    "push = (title, url, type) => sort.push({\n"
    "    title: title,\n"
    "    url: url,\n"
    "    style: {\n"
    "        layout_flexBasisPercent: type\n"
    "    }\n"
    "});\n"
    "\n"
    "push('番茄书架', 'https://fanqienovel.com/reading/bookapi/bookshelf/info/v:version/?aid=1967&iid=0&version_code=56910&update_version_code=56910', 1);\n"
    "push('巅峰榜单', 'https://fanqienovel.com/api/author/misc/top_book_list/v1/?limit=100&offset=0', 0.29);\n"
    "push('出版榜单', 'https://fanqienovel.com/api/node/publication/list?page_index=0&page_count=100', 0.29);\n"
    "push('爆更榜单', 'https://api-lf.fanqiesdk.com/api/novel/channel/homepage/rank/rank_list/v2/?aid=13&limit=50&offset=0&side_type=15&type=1', 0.29);\n"
    "push('黑马榜单', 'https://api-lf.fanqiesdk.com/api/novel/channel/homepage/rank/rank_list/v2/?aid=13&limit=50&offset=0&side_type=13&type=1', 0.29);\n"
    "push('热搜榜单', 'https://api-lf.fanqiesdk.com/api/novel/channel/homepage/rank/rank_list/v2/?aid=13&limit=50&offset=&side_type=12&type=1', 0.29);\n"
    "push('每周推荐', 'https://fanqienovel.com/api/rank/recommend/list?type=1&limit=10&offset=0', 0.29);\n"
    "\n"
    "A = [[\"推荐\",\"10\"],[\"完结\",\"11\"],[\"热搜\",\"12\"]];\n"
    "[[\"男频\",\"1\"],[\"女频\",\"0\"]].map(([title,id]) => {\n"
    "    push('❤️' + title + '榜单❤️', null, 1);\n"
    "    if (id == 2) { A = A.slice(1, A.length-1); }\n"
    "    A.map(([title,uri]) => {\n"
    "        url = `https://novel.snssdk.com/api/novel/channel/homepage/rank/rank_list/v2/?aid=13&app_name=news_article&app_version=9.7.3&channel=tengxun_tt&device_platform=android&os=android&device_type=ProjectTitan&os_api=29&os_version=10&openlive_plugin_status=1&isTTWebViewHeifSupport=0&limit=100&offset={{(page-1)*100}}&side_type=${uri}&type=${id}`;\n"
    "        push(title, url, 0.29);\n"
    "    });\n"
    "});\n"
    "\n"
    "JSON.stringify(sort);\n";

    const std::string result = engine.eval(script);
    std::cout << result << std::endl;
}

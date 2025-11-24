#include <iostream>
#include <string>
#include <booksource/engine.h>
#include <booksource/js_binder.h>

struct Book {
    std::string name;
    std::string author;

    std::string info() const {
        return name + " / " + author;
    }
};

void setupBookBinding(const JSContext *ctx) {
    // 可选：设置类名（用于调试）
    JsBinder<Book>::setClassName("Book");

    // 字段：快照成字符串
    JsBinder<Book>::addField("name",
        [](Book *b) { return b->name; });
    JsBinder<Book>::addField("author",
        [](Book *b) { return b->author; });

    // 方法：返回 JSValue，调用者自由决定返回类型
    JsBinder<Book>::addMethod("info",
        [](JSContext *innerCtx, const Book *b, const int argc, const JSValueConst *argv) -> JSValue {
            (void)argc; (void)argv;
            const std::string s = b->info();
            return JS_NewStringLen(innerCtx, s.c_str(), s.size());
        });

    // 这里只是确保类已经注册；真正 wrap 是在 addObject 时做的
    (void)ctx;
}

int main() {
    const QuickJsEngine engine;

    // 注册 Book 的类信息（只需调用一次）
    setupBookBinding(engine.getContext());

    Book book{"Lord of the Rings", "Tolkien"};

    // 绑定对象到 JS 全局：global.book = <Book>
    engine.addObject("book", &book);

    const std::string r1 = engine.eval("book.name + ' / ' + book.author");
    std::cout << r1 << std::endl;

    const std::string r2 = engine.eval("book.info()");
    std::cout << r2 << std::endl;

    // 绑定字符串类似 Kotlin 里的 bindings["baseUrl"] 之类
    engine.addBinding("baseUrl", "https://example.com");
    std::cout << engine.eval("baseUrl + '/api'") << std::endl;

    return 0;
}
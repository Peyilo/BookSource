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
    test_reset(engine);
    test_bind(engine);
}

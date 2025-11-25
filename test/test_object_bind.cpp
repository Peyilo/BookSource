#include <iostream>
#include <string>
#include <booksource/engine.h>
#include <booksource/bind.h>
#include <cassert>

struct Book {
    std::string name;
    std::string author;

    std::string info() const {
        return name + " / " + author;
    }

    int test(int param1, int param2, Book param3) const {
        std::cout << "param1: " << param1 << std::endl;
        std::cout << "param2: " << param2 << std::endl;
        std::cout << "param3: " << param3.name << "; " << param3.author << std::endl;
        return param1 + param2;
    }

};

void initBookClassInfo() {
    JsBinder<Book>::setClassName("Book");
    JsBinder<Book>::addField("name", &Book::name);
    JsBinder<Book>::addField("author", &Book::author);
    JsBinder<Book>::addMethod("info", &Book::info);
    JsBinder<Book>::addMethod("test", &Book::test);
}

void exeJs(const QuickJsEngine &engine, const std::string &code) {
    const std::string res = engine.eval(code);
    std::cout << "[Result] " << res << std::endl;
}

int main() {
    const QuickJsEngine engine;
    initBookClassInfo();

    Book book{"first book", "first author"};
    Book book2{"second book", "second author"};
    engine.addObjectBinding("book", &book);
    engine.addObjectBinding("book2", &book2);
    engine.addPrintFunc("print");

    // 验证绑定后，能否在js中访问字段和方法
    exeJs(engine, "book.name + ' / ' + book.author");
    exeJs(engine, "book.info()");
    exeJs(engine, "book.test(1, 2, book2)");

    // 验证在js中修改了绑定对象的值以后，cpp中相应的变量是否会修改
    exeJs(engine, "book.name = \"first book (edited)\";");
    exeJs(engine, "book.author = \"first author (edited)\";");
    exeJs(engine, "book.name + ' / ' + book.author");
    assert("first book (edited)" == book.name);
    assert("first author (edited)" == book.author);

    // 验证在cpp中修改了绑定对象的值以后，js中相应的变量是否会修改
    book2.author = "Tolkien";
    exeJs(engine, "book2.info()");
    exeJs(engine, "book2.test(5, 6, book2)");

    // 检查基础类型的绑定是否为双向的
    // TODO: 不是双向的，实际上是
    int num = 123;
    engine.addValueBinding("num", &num);
    exeJs(engine, "num = 456;");
    std::cout << "num: " << num << std::endl;
    num = 789;
    // engine.deleteValue("num");
    exeJs(engine, "num;");

    return 0;
}

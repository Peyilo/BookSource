#include <iostream>
#include <string>
#include <booksource/engine.h>
#include <booksource/bind.h>

struct Book {
    std::string name;
    std::string author;

    std::string info() const {
        return name + " / " + author;
    }

    int test(int param1, int param2, Book param3) const {
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
    std::cout << res << std::endl;
}

int main() {
    const QuickJsEngine engine;
    initBookClassInfo();

    Book book{"Lord of the Rings", "Tolkien"};
    engine.addObject("book", &book);

    exeJs(engine, "book.name + ' / ' + book.author");
    exeJs(engine, "book.name = \"American\";");
    exeJs(engine, "book.author = \"Peyilo\";");
    exeJs(engine, "book.name + ' / ' + book.author");

    exeJs(engine, "book.info()");
    exeJs(engine, "book.test(1, 2, book)");

    return 0;
}

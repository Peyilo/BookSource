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
};

void initBookClassInfo() {
    JsBinder<Book>::setClassName("Book");

    JsBinder<Book>::addField<std::string>(
        "name",
        [](Book *b) -> std::string { return b->name; },
        [](Book *b, const std::string &v) { b->name = v; }
    );
    JsBinder<Book>::addField<std::string>(
        "author",
        [](Book *b) -> std::string { return b->author; },
        [](Book *b, const std::string &v) { b->author = v; }
    );
}

void exeJs(const QuickJsEngine &engine, const std::string& code) {
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

    return 0;
}

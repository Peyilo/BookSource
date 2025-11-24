#include <iostream>
#include <booksource/js_utils.h>

int main() {
    try {
        std::cout << "Test 1: 1+2*3 = "
                  << run_simple_js("1 + 2 * 3") << std::endl;

        std::cout << "Test 2: JSON.stringify"
                  << run_simple_js("JSON.stringify({x:10, y:20})")
                  << std::endl;

        std::cout << "Test 3: undefined -> "
                  << run_simple_js("undefined")
                  << std::endl;

        std::cout << "Test 4: [1+1;]"
                 << run_simple_js("1+1;")
                 << std::endl;

        std::cout << "Test 5: exception test" << std::endl;
        run_simple_js("throw new Error('boom')");

    }
    catch (const std::exception& e) {
        std::cout << "JS Error: " << e.what() << std::endl;
    }

    return 0;
}

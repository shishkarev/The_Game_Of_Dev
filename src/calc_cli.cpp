#include "calc/lexer.h"
#include "calc/parser.h"
#include "calc/eval.h"

#include <iostream>
#include <string>

int main(int argc, char** argv) {
    std::string input;
    if (argc >= 2) {
        input = argv[1];
    } else {
        std::getline(std::cin, input);
    }

    try {
        calc::Lexer lex(input);
        calc::Parser parser(std::move(lex));
        auto ast = parser.parse();
        double result = calc::eval(*ast);
        std::cout << result << "\n";
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << "\n";
        return 1;
    }
}

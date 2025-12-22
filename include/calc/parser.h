#pragma once
#include "calc/lexer.h"
#include "calc/ast.h"
#include <memory>

namespace calc {

class Parser {
public:
    explicit Parser(Lexer lexer);
    std::unique_ptr<Expr> parse();

private:
    Lexer lexer_;
    Token cur_;

    void advance();
    void expect(TokenType type, const char* message);

    std::unique_ptr<Expr> parse_expression();
    std::unique_ptr<Expr> parse_term();
    std::unique_ptr<Expr> parse_factor();
};

} // namespace calc

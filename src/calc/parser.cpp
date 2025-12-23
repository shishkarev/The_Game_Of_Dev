#include "calc/parser.h"
#include <stdexcept>

namespace calc {

Parser::Parser(Lexer lexer) : lexer_(std::move(lexer)) {
    advance();
}

void Parser::advance() {
    cur_ = lexer_.next();
}

void Parser::expect(TokenType type, const char* message) {
    if (cur_.type != type) {
        throw std::runtime_error(std::string(message) + " at position " + std::to_string(cur_.pos));
    }
}

Program Parser::parse() {
    Program p = parse_program();

    if (cur_.type != TokenType::End) {
        throw std::runtime_error(
            "Unexpected token '" + cur_.lexeme + "' at position " + std::to_string(cur_.pos)
        );
    }
    return p;
}

std::unique_ptr<Expr> Parser::parse_expression() {
    auto left = parse_term();

    while (cur_.type == TokenType::Plus || cur_.type == TokenType::Minus) {
        Token op = cur_;
        advance();
        auto right = parse_term();

        if (op.type == TokenType::Plus) {
            left = std::make_unique<BinaryExpr>(BinOp::Add, std::move(left), std::move(right));
        } else {
            left = std::make_unique<BinaryExpr>(BinOp::Sub, std::move(left), std::move(right));
        }
    }

    return left;
}

std::unique_ptr<Expr> Parser::parse_term() {
    auto left = parse_factor();

    while (cur_.type == TokenType::Star || cur_.type == TokenType::Slash) {
        Token op = cur_;
        advance();
        auto right = parse_factor();

        if (op.type == TokenType::Star) {
            left = std::make_unique<BinaryExpr>(BinOp::Mul, std::move(left), std::move(right));
        } else {
            left = std::make_unique<BinaryExpr>(BinOp::Div, std::move(left), std::move(right));
        }
    }

    return left;
}

std::unique_ptr<Expr> Parser::parse_factor() {
    if (cur_.type == TokenType::Plus || cur_.type == TokenType::Minus) {
        char op = (cur_.type == TokenType::Plus) ? '+' : '-';
        advance();
        auto rhs = parse_factor();
        return std::make_unique<UnaryExpr>(op, std::move(rhs));
    }

    if (cur_.type == TokenType::Number) {
        double v = cur_.number;
        advance();
        return std::make_unique<NumberExpr>(v);
    }

    if (cur_.type == TokenType::LParen) {
        advance();
        auto inside = parse_expression();
        expect(TokenType::RParen, "Expected ')'");
        advance();
        return inside;
    }

    if (cur_.type == TokenType::Ident) {
        std::string name = cur_.lexeme;
        advance();
        return std::make_unique<VarExpr>(std::move(name));
    }

    throw std::runtime_error("Expected number or '(' at position " + std::to_string(cur_.pos));
}

Program Parser::parse_program() {
    Program p;

    if (cur_.type == TokenType::End) return p;

    p.stmts.push_back(parse_stmt());

    while (cur_.type == TokenType::Semicolon) {
        advance();
        if (cur_.type == TokenType::End) break;
        p.stmts.push_back(parse_stmt());
    }

    return p;
}

std::unique_ptr<Stmt> Parser::parse_stmt() {
    if (cur_.type == TokenType::Ident) {
        Token ident = cur_;
        Token next = lexer_.peek();

        if (next.type == TokenType::Equal) {
            advance();
            advance();
            auto value = parse_expression();
            return std::make_unique<AssignStmt>(ident.lexeme, std::move(value));
        }
    }

    auto e = parse_expression();
    return std::make_unique<ExprStmt>(std::move(e));
}

} // namespace calc

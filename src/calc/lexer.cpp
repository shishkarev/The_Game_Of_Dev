#include "calc/lexer.h"
#include <cctype>
#include <stdexcept>

namespace calc {

Lexer::Lexer(std::string input) : input_(std::move(input)) {}

char Lexer::current() const {
    return eof() ? '\0' : input_[i_];
}

bool Lexer::eof() const {
    return i_ >= input_.size();
}

void Lexer::skip_spaces() {
    while (!eof() && std::isspace(static_cast<unsigned char>(current()))) {
        ++i_;
    }
}

Token Lexer::read_number() {
    std::size_t start = i_;
    bool seen_dot = false;

    while (!eof()) {
        char c = current();
        if (std::isdigit(static_cast<unsigned char>(c))) {
            ++i_;
        } else if (c == '.' && !seen_dot) {
            seen_dot = true;
            ++i_;
        } else {
            break;
        }
    }

    Token t;
    t.type = TokenType::Number;
    t.pos = start;
    t.lexeme = input_.substr(start, i_ - start);

    try {
        t.number = std::stod(t.lexeme);
    } catch (...) {
        throw std::runtime_error("Invalid number at position " + std::to_string(start));
    }

    return t;
}

Token Lexer::next() {
    if (has_peek_) {
        has_peek_ = false;
        return peeked_;
    }

    skip_spaces();

    Token t;
    t.pos = i_;

    if (eof()) {
        t.type = TokenType::End;
        t.lexeme = "";
        return t;
    }

    char c = current();

    if (std::isdigit(static_cast<unsigned char>(c)) || c == '.') {
        return read_number();
    }

    ++i_;

    switch (c) {
        case '+': t.type = TokenType::Plus;  t.lexeme = "+"; return t;
        case '-': t.type = TokenType::Minus; t.lexeme = "-"; return t;
        case '*': t.type = TokenType::Star;  t.lexeme = "*"; return t;
        case '/': t.type = TokenType::Slash; t.lexeme = "/"; return t;
        case '(': t.type = TokenType::LParen; t.lexeme = "("; return t;
        case ')': t.type = TokenType::RParen; t.lexeme = ")"; return t;
        default:
            throw std::runtime_error(
                "Unexpected character '" + std::string(1, c) +
                "' at position " + std::to_string(t.pos)
            );
    }
}

Token Lexer::peek() {
    if (!has_peek_) {
        peeked_ = next();
        has_peek_ = true;
    }
    return peeked_;
}

} // namespace calc

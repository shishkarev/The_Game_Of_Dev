#pragma once
#include <string>
#include <cstddef>

namespace calc{
enum class TokenType{
    Number,
    Ident,
    Equal,
    Semicolon,
    Plus,
    Minus,
    Star,
    Slash,
    LParen,
    RParen,
    End
};

struct Token{
    TokenType type{};
    std::string lexeme;
    double number = 0.0;
    std::size_t pos = 0;
};

} // namespace calc
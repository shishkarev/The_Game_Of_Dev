#pragma once
#include "calc/token.h"
#include <string>

namespace calc{
class Lexer{
public:
    explicit Lexer(std::string input);

    Token next();
    Token peek();
private:
    std::string input_;
    std::size_t i_ = 0;
    bool has_peek_ = false;
    Token peeked_{};

    void skip_spaces();
    Token read_number();
    char current() const;
    bool eof() const;
};

} // namespace calc
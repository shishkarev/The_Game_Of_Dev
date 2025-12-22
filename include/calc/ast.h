#pragma once
#include <memory>

namespace calc {

enum class BinOp { Add, Sub, Mul, Div };

struct Expr {
    virtual ~Expr() = default;
};

struct NumberExpr : Expr {
    double value;
    explicit NumberExpr(double v) : value(v) {}
};

struct UnaryExpr : Expr {
    char op;
    std::unique_ptr<Expr> rhs;
    UnaryExpr(char o, std::unique_ptr<Expr> r) : op(o), rhs(std::move(r)) {}
};

struct BinaryExpr : Expr {
    BinOp op;
    std::unique_ptr<Expr> lhs;
    std::unique_ptr<Expr> rhs;
    BinaryExpr(BinOp o, std::unique_ptr<Expr> l, std::unique_ptr<Expr> r)
        : op(o), lhs(std::move(l)), rhs(std::move(r)) {}
};

} // namespace calc

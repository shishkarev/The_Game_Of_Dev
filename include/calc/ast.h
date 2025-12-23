#pragma once
#include <memory>
#include <string>
#include <vector>

namespace calc {

enum class BinOp { Add, Sub, Mul, Div };

struct Expr {
    virtual ~Expr() = default;
};

struct NumberExpr : Expr {
    double value;
    explicit NumberExpr(double v) : value(v) {}
};

struct VarExpr : Expr {
    std::string name;
    explicit VarExpr(std::string n) : name(std::move(n)) {}
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

struct Stmt {
    virtual ~Stmt() = default;
};

struct ExprStmt : Stmt {
    std::unique_ptr<Expr> expr;
    explicit ExprStmt(std::unique_ptr<Expr> e) : expr(std::move(e)) {}
};

struct AssignStmt : Stmt {
    std::string name;
    std::unique_ptr<Expr> value;
    AssignStmt(std::string n, std::unique_ptr<Expr> v)
        : name(std::move(n)), value(std::move(v)) {}
};

struct Program {
    std::vector<std::unique_ptr<Stmt>> stmts;
};

} // namespace calc

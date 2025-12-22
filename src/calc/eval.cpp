#include "calc/eval.h"
#include <stdexcept>

namespace calc {

double eval(const Expr& e) {
    if (auto n = dynamic_cast<const NumberExpr*>(&e)) {
        return n->value;
    }
    if (auto u = dynamic_cast<const UnaryExpr*>(&e)) {
        double v = eval(*u->rhs);
        if (u->op == '+') return v;
        if (u->op == '-') return -v;
        throw std::runtime_error("Unknown unary operator");
    }
    if (auto b = dynamic_cast<const BinaryExpr*>(&e)) {
        double l = eval(*b->lhs);
        double r = eval(*b->rhs);

        switch (b->op) {
            case BinOp::Add: return l + r;
            case BinOp::Sub: return l - r;
            case BinOp::Mul: return l * r;
            case BinOp::Div:
                if (r == 0.0) throw std::runtime_error("Division by zero");
                return l / r;
        }
    }
    throw std::runtime_error("Unknown expression node");
}

} // namespace calc

#include "calc/eval.h"
#include <stdexcept>

namespace calc {

double eval(const Expr& e, Env& env) {
    if (auto* n = dynamic_cast<const NumberExpr*>(&e)) {
        return n->value;
    }

    if (auto* v = dynamic_cast<const VarExpr*>(&e)) {
        auto it = env.find(v->name);
        if (it == env.end()) {
            throw std::runtime_error("Unknown variable '" + v->name + "'");
        }
        return it->second;
    }

    if (auto* u = dynamic_cast<const UnaryExpr*>(&e)) {
        double x = eval(*u->rhs, env);
        if (u->op == '+') return x;
        if (u->op == '-') return -x;
        throw std::runtime_error("Unknown unary operator");
    }

    if (auto* b = dynamic_cast<const BinaryExpr*>(&e)) {
        double l = eval(*b->lhs, env);
        double r = eval(*b->rhs, env);

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

double eval(const Program& p, Env& env) {
    double last = 0.0;

    for (const auto& st : p.stmts) {
        if (auto* a = dynamic_cast<const AssignStmt*>(st.get())) {
            double v = eval(*a->value, env);
            env[a->name] = v;
            last = v;
        } else if (auto* es = dynamic_cast<const ExprStmt*>(st.get())) {
            last = eval(*es->expr, env);
        } else {
            throw std::runtime_error("Unknown statement node");
        }
    }

    return last;
}

double eval(const Expr& e) {
    Env env;
    return eval(e, env);
}

} // namespace calc

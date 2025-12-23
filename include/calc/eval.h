#pragma once
#include <string>
#include <unordered_map>

#include "calc/ast.h"

namespace calc {

using Env = std::unordered_map<std::string, double>;

double eval(const Expr& e, Env& env);
double eval(const Program& p, Env& env);
double eval(const Expr& e);

} // namespace calc

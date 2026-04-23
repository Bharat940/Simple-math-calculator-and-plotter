#pragma once
#include <vector>
#include <string>
#include "tokenizer.h"
#include "result.h"

class Expression
{
public:
    explicit Expression(const std::string &expr);

    EvalResult validate() const;
    EvalResult evalSafe(double x) const;
    double eval(double x) const;

private:
    std::vector<Token> postfix_;
};

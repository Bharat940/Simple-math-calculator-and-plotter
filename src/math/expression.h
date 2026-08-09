#pragma once
#include <string>
#include <memory>
#include "result.h"
#include "../compiler/ast/Node.hpp"

class Expression
{
public:
    Expression() = default;
    explicit Expression(const std::string &expr);

    EvalResult validate() const;
    EvalResult evalSafe(double x) const;
    double eval(double x) const;
    double evalSignal(double xVal, double tVal = 0.0, double nVal = 0.0) const;

    const ASTNode *getAST() const { return astRoot_.get(); }
    bool isValid() const { return valid_; }

private:
    std::string rawExpression_;
    std::shared_ptr<ASTNode> astRoot_;
    bool valid_ = false;
};

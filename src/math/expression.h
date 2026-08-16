#pragma once
#include <string>
#include <memory>
#include "result.h"
#include "../compiler/ast/Node.hpp"
#include "../compiler/diagnostics/Diagnostics.hpp"

namespace mathstudio::core
{
    struct EvaluationContext;
}

class Expression
{
public:
    Expression() = default;
    // Parse using an internal (throw-away) reporter – throws on error
    explicit Expression(const std::string &expr);
    // Parse using a shared reporter – does NOT throw; check isValid() after
    Expression(const std::string &expr, DiagnosticsReporter &reporter, const std::string &label = "");

    EvalResult validate() const;
    EvalResult evalSafe(double x) const;
    double eval(double x) const;
    double evalSignal(double xVal, double tVal = 0.0, double nVal = 0.0) const;

    // Evaluation overloads preserving shared EvaluationContext (user parameters & sliders)
    double evalWithContext(double x, mathstudio::core::EvaluationContext &context) const;
    double evalSignalWithContext(double xVal, double tVal, double nVal, mathstudio::core::EvaluationContext &context) const;

    const ASTNode *getAST() const { return astRoot_.get(); }
    bool isValid() const { return valid_; }

private:
    std::string rawExpression_;
    std::shared_ptr<ASTNode> astRoot_;
    bool valid_ = false;
};

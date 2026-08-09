#include "expression.h"
#include "../compiler/parser/PrattParser.hpp"
#include "../compiler/visitors/EvalVisitor.hpp"
#include "../compiler/passes/ConstantFoldVisitor.hpp"
#include "../compiler/passes/SimplifyVisitor.hpp"
#include "../compiler/diagnostics/Diagnostics.hpp"
#include "../core/VariableStore.hpp"
#include "../core/FunctionRegistry.hpp"
#include "../core/EvaluationContext.hpp"
#include "../core/ExpressionCache.hpp"
#include <stdexcept>
#include <limits>

using namespace mathstudio::core;

Expression::Expression(const std::string &expr) : rawExpression_(expr)
{
    if (expr.empty())
    {
        valid_ = false;
        throw std::runtime_error("Empty expression");
    }

    // 1. Check ExpressionCache
    auto cachedAST = ExpressionCache::instance().get(expr);
    if (cachedAST)
    {
        astRoot_ = std::move(cachedAST);
        valid_ = true;
        return;
    }

    // 2. Tokenize and parse via PrattParser
    auto tokens = tokenize(expr);
    DiagnosticsReporter diagnostics;
    PrattParser parser(tokens, diagnostics);
    auto ast = parser.parse();

    if (diagnostics.hasErrors() || !ast)
    {
        valid_ = false;
        std::string err = diagnostics.hasErrors() ? diagnostics.getErrors()[0].message : "Invalid expression";
        throw std::runtime_error(err);
    }

    // 3. PassManager optimization passes: Constant Folding + Simplify Visitor
    ConstantFoldVisitor foldVisitor;
    ast = foldVisitor.run(std::move(ast));

    SimplifyVisitor simplifyVisitor;
    ast = simplifyVisitor.run(std::move(ast));

    astRoot_ = std::move(ast);
    valid_ = true;

    // Cache compiled AST root
    ExpressionCache::instance().store(expr, astRoot_.get());
}

EvalResult Expression::validate() const
{
    if (!valid_ || !astRoot_)
    {
        return EvalResult::failure("Invalid expression AST");
    }
    return EvalResult::success(0.0);
}

EvalResult Expression::evalSafe(double x) const
{
    if (!valid_ || !astRoot_)
    {
        return EvalResult::failure("Invalid expression AST");
    }

    VariableStore vars;
    vars.set("x", MathValue::real(x));
    vars.set("t", MathValue::real(x));
    vars.set("n", MathValue::real(x));
    vars.set("theta", MathValue::real(x));
    vars.set("z", MathValue::real(x));
    const auto &fns = FunctionRegistry::instance();
    DiagnosticsReporter diag;
    EvaluationContext ctx(vars, fns, diag);

    EvalVisitor visitor(ctx);
    auto nodeCopy = astRoot_->clone();
    nodeCopy->accept(&visitor);

    if (diag.hasErrors())
    {
        return EvalResult::failure(diag.getErrors()[0].message);
    }

    MathValue res = visitor.getResult();
    if (res.isUndef)
    {
        return EvalResult::failure("Undefined result");
    }
    return EvalResult::success(res.getReal());
}

double Expression::eval(double x) const
{
    auto r = evalSafe(x);
    return r.ok ? r.value : std::numeric_limits<double>::quiet_NaN();
}

double Expression::evalSignal(double xVal, double tVal, double nVal) const
{
    if (!valid_ || !astRoot_)
        return std::numeric_limits<double>::quiet_NaN();

    VariableStore vars;
    vars.set("x", MathValue::real(xVal));
    vars.set("t", MathValue::real(tVal));
    vars.set("n", MathValue::real(nVal));
    const auto &fns = FunctionRegistry::instance();
    DiagnosticsReporter diag;
    EvaluationContext ctx(vars, fns, diag);

    EvalVisitor visitor(ctx);
    auto nodeCopy = astRoot_->clone();
    nodeCopy->accept(&visitor);

    MathValue res = visitor.getResult();
    return res.isUndef ? std::numeric_limits<double>::quiet_NaN() : res.getReal();
}

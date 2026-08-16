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

    auto cachedAST = ExpressionCache::instance().get(expr);
    if (cachedAST)
    {
        astRoot_ = std::move(cachedAST);
        valid_ = true;
        return;
    }

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

    ConstantFoldVisitor foldVisitor;
    ast = foldVisitor.run(std::move(ast));
    SimplifyVisitor simplifyVisitor;
    ast = simplifyVisitor.run(std::move(ast));

    astRoot_ = std::move(ast);
    valid_ = true;
    ExpressionCache::instance().store(expr, astRoot_.get());
}

// Shared-reporter variant: does NOT throw; surfaces errors into the shared DiagnosticsReporter
Expression::Expression(const std::string &expr, DiagnosticsReporter &reporter, const std::string &label)
    : rawExpression_(expr)
{
    if (expr.empty())
    {
        valid_ = false;
        reporter.reportError("Empty expression" + (label.empty() ? "" : " in '" + label + "'"), 1, 0);
        return;
    }

    auto cachedAST = ExpressionCache::instance().get(expr);
    if (cachedAST)
    {
        astRoot_ = std::move(cachedAST);
        valid_ = true;
        return;
    }

    auto tokens = tokenize(expr);
    DiagnosticsReporter localDiag;
    PrattParser parser(tokens, localDiag);
    auto ast = parser.parse();

    if (localDiag.hasErrors() || !ast)
    {
        valid_ = false;
        for (const auto &err : localDiag.getErrors())
        {
            std::string msg = label.empty() ? err.message : "[" + label + "] " + err.message;
            reporter.reportError(msg, err.line, err.column, err.hint);
        }
        if (!localDiag.hasErrors())
        {
            std::string msg = label.empty() ? "Invalid expression" : "[" + label + "] Invalid expression";
            reporter.reportError(msg, 1, 0);
        }
        return;
    }

    ConstantFoldVisitor foldVisitor;
    ast = foldVisitor.run(std::move(ast));
    SimplifyVisitor simplifyVisitor;
    ast = simplifyVisitor.run(std::move(ast));

    astRoot_ = std::move(ast);
    valid_ = true;
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
    if (res.isComplex())
    {
        auto c = res.getComplex();
        if (std::abs(c.imag()) > 1e-12)
        {
            return EvalResult::failure("Complex result with non-zero imaginary part");
        }
        return EvalResult::success(c.real());
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
    if (res.isUndef)
        return std::numeric_limits<double>::quiet_NaN();
    if (res.isComplex())
    {
        auto c = res.getComplex();
        return (std::abs(c.imag()) > 1e-12) ? std::numeric_limits<double>::quiet_NaN() : c.real();
    }
    return res.getReal();
}

double Expression::evalWithContext(double x, EvaluationContext &context) const
{
    if (!valid_ || !astRoot_)
        return std::numeric_limits<double>::quiet_NaN();

    context.vars.set("x", MathValue::real(x));
    if (!context.vars.has("t"))
        context.vars.set("t", MathValue::real(x));
    if (!context.vars.has("n"))
        context.vars.set("n", MathValue::real(x));
    if (!context.vars.has("theta"))
        context.vars.set("theta", MathValue::real(x));
    if (!context.vars.has("z"))
        context.vars.set("z", MathValue::real(x));

    EvalVisitor visitor(context);
    auto nodeCopy = astRoot_->clone();
    nodeCopy->accept(&visitor);

    MathValue res = visitor.getResult();
    if (res.isUndef)
        return std::numeric_limits<double>::quiet_NaN();
    if (res.isComplex())
    {
        auto c = res.getComplex();
        return (std::abs(c.imag()) > 1e-12) ? std::numeric_limits<double>::quiet_NaN() : c.real();
    }
    return res.getReal();
}

double Expression::evalSignalWithContext(double xVal, double tVal, double nVal, EvaluationContext &context) const
{
    if (!valid_ || !astRoot_)
        return std::numeric_limits<double>::quiet_NaN();

    context.vars.set("x", MathValue::real(xVal));
    if (!context.vars.has("t"))
        context.vars.set("t", MathValue::real(tVal));
    if (!context.vars.has("n"))
        context.vars.set("n", MathValue::real(nVal));

    EvalVisitor visitor(context);
    auto nodeCopy = astRoot_->clone();
    nodeCopy->accept(&visitor);

    MathValue res = visitor.getResult();
    if (res.isUndef)
        return std::numeric_limits<double>::quiet_NaN();
    if (res.isComplex())
    {
        auto c = res.getComplex();
        return (std::abs(c.imag()) > 1e-12) ? std::numeric_limits<double>::quiet_NaN() : c.real();
    }
    return res.getReal();
}

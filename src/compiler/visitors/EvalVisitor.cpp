#include "EvalVisitor.hpp"
#include <cmath>

void EvalVisitor::visit(NumberNode *node)
{
    result = MathValue::real(node->value);
}

void EvalVisitor::visit(VariableNode *node)
{
    result = context.vars.get(node->name);
    if (result.isUndef && !context.vars.has(node->name))
    {
        // Industry Standard Fallback (Desmos): default uninitialized parameters to 1.0
        // so the graph renders immediately without leaving a blank canvas!
        result = MathValue::real(1.0);
    }
}

void EvalVisitor::visit(ConstantNode *node)
{
    result = MathValue::real(node->value);
}

void EvalVisitor::visit(BinaryOpNode *node)
{
    if (!node->left || !node->right)
    {
        result = MathValue::undefined();
        return;
    }

    node->left->accept(this);
    MathValue leftVal = result;

    node->right->accept(this);
    MathValue rightVal = result;

    switch (node->opType)
    {
    case BinaryOpType::Add:
        result = leftVal + rightVal;
        return;
    case BinaryOpType::Sub:
        result = leftVal - rightVal;
        return;
    case BinaryOpType::Mul:
        result = leftVal * rightVal;
        return;
    case BinaryOpType::Div:
        if (rightVal.getReal() == 0.0 && !rightVal.isComplex())
        {
            context.diagnostics.reportError("Division by zero", 1, 0);
            result = MathValue::undefined();
        }
        else
        {
            result = leftVal / rightVal;
        }
        return;
    case BinaryOpType::Pow:
        if (leftVal.isComplex() || rightVal.isComplex())
        {
            result = MathValue::complex(std::pow(leftVal.getComplex(), rightVal.getComplex()));
        }
        else
        {
            result = MathValue::real(std::pow(leftVal.getReal(), rightVal.getReal()));
        }
        return;
    default:
        result = MathValue::undefined();
        return;
    }
}

void EvalVisitor::visit(UnaryOpNode *node)
{
    if (!node->operand)
    {
        result = MathValue::undefined();
        return;
    }
    node->operand->accept(this);
    if (node->opType == UnaryOpType::Negate)
    {
        result = -result;
    }
}

void EvalVisitor::visit(FunctionNode *node)
{
    std::vector<MathValue> args;
    args.reserve(node->arguments.size());
    for (const auto &arg : node->arguments)
    {
        if (arg)
        {
            arg->accept(this);
            args.push_back(result);
        }
    }
    if (context.functions.has(node->name))
    {
        result = context.functions.call(node->name, args);
    }
    else
    {
        context.diagnostics.reportError("Unknown function: " + node->name, 1, 0);
        result = MathValue::undefined();
    }
}

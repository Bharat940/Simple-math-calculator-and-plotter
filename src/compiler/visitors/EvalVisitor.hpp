#pragma once

#include "Visitor.hpp"
#include "../ast/Node.hpp"
#include "../../core/MathValue.hpp"
#include "../../core/EvaluationContext.hpp"

class EvalVisitor : public Visitor
{
private:
    EvaluationContext &context;
    MathValue result;

public:
    explicit EvalVisitor(EvaluationContext &ctx) : context(ctx) {}

    MathValue getResult() const { return result; }

    void visit(NumberNode *node) override;
    void visit(VariableNode *node) override;
    void visit(ConstantNode *node) override;
    void visit(BinaryOpNode *node) override;
    void visit(UnaryOpNode *node) override;
    void visit(FunctionNode *node) override;
};

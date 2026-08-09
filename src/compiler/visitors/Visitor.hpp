#pragma once

class NumberNode;
class VariableNode;
class ConstantNode;
class BinaryOpNode;
class UnaryOpNode;
class FunctionNode;

class Visitor
{
public:
    virtual ~Visitor() = default;
    virtual void visit(NumberNode *node) = 0;
    virtual void visit(VariableNode *node) = 0;
    virtual void visit(ConstantNode *node) = 0;
    virtual void visit(BinaryOpNode *node) = 0;
    virtual void visit(UnaryOpNode *node) = 0;
    virtual void visit(FunctionNode *node) = 0;
};

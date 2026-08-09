#pragma once

#include "Visitor.hpp"
#include "../ast/Node.hpp"
#include <string>

class PrintVisitor : public Visitor
{
private:
    std::string result;

public:
    std::string getResult() const { return result; }

    void visit(NumberNode *node) override;
    void visit(VariableNode *node) override;
    void visit(ConstantNode *node) override;
    void visit(BinaryOpNode *node) override;
    void visit(UnaryOpNode *node) override;
    void visit(FunctionNode *node) override;
};

#pragma once

#include "../visitors/Visitor.hpp"
#include "../ast/Node.hpp"
#include "PassManager.hpp"
#include <memory>

class ConstantFoldVisitor : public Visitor, public ASTPass
{
private:
    std::unique_ptr<ASTNode> foldedNode;

public:
    ConstantFoldVisitor() = default;
    std::unique_ptr<ASTNode> run(std::unique_ptr<ASTNode> node) override;
    std::unique_ptr<ASTNode> getFoldedNode();

    void visit(NumberNode *node) override;
    void visit(VariableNode *node) override;
    void visit(ConstantNode *node) override;
    void visit(BinaryOpNode *node) override;
    void visit(UnaryOpNode *node) override;
    void visit(FunctionNode *node) override;
};

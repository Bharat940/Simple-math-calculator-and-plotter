#pragma once

#include "../visitors/Visitor.hpp"
#include "../ast/Node.hpp"
#include "PassManager.hpp"
#include <memory>

class SimplifyVisitor : public Visitor, public ASTPass
{
private:
    std::unique_ptr<ASTNode> simplifiedNode;

    bool isNumber(const ASTNode *node, double targetVal) const;
    bool isZero(const ASTNode *node) const { return isNumber(node, 0.0); }
    bool isOne(const ASTNode *node) const { return isNumber(node, 1.0); }

public:
    SimplifyVisitor() = default;
    std::unique_ptr<ASTNode> run(std::unique_ptr<ASTNode> node) override;
    std::unique_ptr<ASTNode> getSimplifiedNode();

    void visit(NumberNode *node) override;
    void visit(VariableNode *node) override;
    void visit(ConstantNode *node) override;
    void visit(BinaryOpNode *node) override;
    void visit(UnaryOpNode *node) override;
    void visit(FunctionNode *node) override;
};

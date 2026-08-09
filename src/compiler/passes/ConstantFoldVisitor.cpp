#include "ConstantFoldVisitor.hpp"
#include <cmath>

std::unique_ptr<ASTNode> ConstantFoldVisitor::run(std::unique_ptr<ASTNode> node)
{
    if (node)
    {
        node->accept(this);
        return std::move(foldedNode);
    }
    return nullptr;
}

std::unique_ptr<ASTNode> ConstantFoldVisitor::getFoldedNode()
{
    return std::move(foldedNode);
}

void ConstantFoldVisitor::visit(NumberNode *node)
{
    foldedNode = node->clone();
}

void ConstantFoldVisitor::visit(VariableNode *node)
{
    foldedNode = node->clone();
}

void ConstantFoldVisitor::visit(ConstantNode *node)
{
    foldedNode = node->clone();
}

void ConstantFoldVisitor::visit(BinaryOpNode *node)
{
    std::unique_ptr<ASTNode> leftFolded, rightFolded;
    if (node->left)
    {
        node->left->accept(this);
        leftFolded = std::move(foldedNode);
    }
    if (node->right)
    {
        node->right->accept(this);
        rightFolded = std::move(foldedNode);
    }

    auto numLeft = dynamic_cast<NumberNode *>(leftFolded.get());
    auto numRight = dynamic_cast<NumberNode *>(rightFolded.get());

    if (numLeft && numRight)
    {
        double l = numLeft->value;
        double r = numRight->value;
        switch (node->opType)
        {
        case BinaryOpType::Add:
            foldedNode = std::make_unique<NumberNode>(l + r);
            return;
        case BinaryOpType::Sub:
            foldedNode = std::make_unique<NumberNode>(l - r);
            return;
        case BinaryOpType::Mul:
            foldedNode = std::make_unique<NumberNode>(l * r);
            return;
        case BinaryOpType::Div:
            if (r != 0.0)
            {
                foldedNode = std::make_unique<NumberNode>(l / r);
                return;
            }
            break;
        case BinaryOpType::Pow:
            foldedNode = std::make_unique<NumberNode>(std::pow(l, r));
            return;
        default:
            break;
        }
    }
    foldedNode = std::make_unique<BinaryOpNode>(node->opType, std::move(leftFolded), std::move(rightFolded));
}

void ConstantFoldVisitor::visit(UnaryOpNode *node)
{
    std::unique_ptr<ASTNode> opndFolded;
    if (node->operand)
    {
        node->operand->accept(this);
        opndFolded = std::move(foldedNode);
    }
    auto num = dynamic_cast<NumberNode *>(opndFolded.get());
    if (num && node->opType == UnaryOpType::Negate)
    {
        foldedNode = std::make_unique<NumberNode>(-num->value);
    }
    else
    {
        foldedNode = std::make_unique<UnaryOpNode>(node->opType, std::move(opndFolded));
    }
}

void ConstantFoldVisitor::visit(FunctionNode *node)
{
    std::vector<std::unique_ptr<ASTNode>> foldedArgs;
    foldedArgs.reserve(node->arguments.size());
    for (const auto &arg : node->arguments)
    {
        if (arg)
        {
            arg->accept(this);
            foldedArgs.push_back(std::move(foldedNode));
        }
    }
    foldedNode = std::make_unique<FunctionNode>(node->name, std::move(foldedArgs));
}

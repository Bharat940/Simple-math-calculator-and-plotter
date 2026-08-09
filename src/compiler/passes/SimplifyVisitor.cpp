#include "SimplifyVisitor.hpp"
#include <cmath>
#include <limits>

std::unique_ptr<ASTNode> SimplifyVisitor::run(std::unique_ptr<ASTNode> node)
{
    if (node)
    {
        node->accept(this);
        return std::move(simplifiedNode);
    }
    return nullptr;
}

std::unique_ptr<ASTNode> SimplifyVisitor::getSimplifiedNode()
{
    return std::move(simplifiedNode);
}

bool SimplifyVisitor::isNumber(const ASTNode *node, double targetVal) const
{
    if (auto num = dynamic_cast<const NumberNode *>(node))
    {
        return std::abs(num->value - targetVal) < 1e-12;
    }
    return false;
}

void SimplifyVisitor::visit(NumberNode *node)
{
    simplifiedNode = node->clone();
}

void SimplifyVisitor::visit(VariableNode *node)
{
    simplifiedNode = node->clone();
}

void SimplifyVisitor::visit(ConstantNode *node)
{
    simplifiedNode = node->clone();
}

void SimplifyVisitor::visit(BinaryOpNode *node)
{
    std::unique_ptr<ASTNode> leftSimp, rightSimp;
    if (node->left)
    {
        node->left->accept(this);
        leftSimp = std::move(simplifiedNode);
    }
    if (node->right)
    {
        node->right->accept(this);
        rightSimp = std::move(simplifiedNode);
    }

    BinaryOpType op = node->opType;

    // 1. Check if both are numbers for instant evaluation
    auto numLeft = dynamic_cast<NumberNode *>(leftSimp.get());
    auto numRight = dynamic_cast<NumberNode *>(rightSimp.get());

    if (numLeft && numRight)
    {
        double l = numLeft->value;
        double r = numRight->value;
        switch (op)
        {
        case BinaryOpType::Add:
            simplifiedNode = std::make_unique<NumberNode>(l + r);
            return;
        case BinaryOpType::Sub:
            simplifiedNode = std::make_unique<NumberNode>(l - r);
            return;
        case BinaryOpType::Mul:
            simplifiedNode = std::make_unique<NumberNode>(l * r);
            return;
        case BinaryOpType::Div:
            if (r != 0.0)
            {
                simplifiedNode = std::make_unique<NumberNode>(l / r);
                return;
            }
            break;
        case BinaryOpType::Pow:
            simplifiedNode = std::make_unique<NumberNode>(std::pow(l, r));
            return;
        default:
            break;
        }
    }

    // 2. Addition identities
    if (op == BinaryOpType::Add)
    {
        if (isZero(leftSimp.get()))
        {
            simplifiedNode = std::move(rightSimp);
            return;
        }
        if (isZero(rightSimp.get()))
        {
            simplifiedNode = std::move(leftSimp);
            return;
        }
    }

    // 3. Subtraction identities
    if (op == BinaryOpType::Sub)
    {
        if (isZero(rightSimp.get()))
        {
            simplifiedNode = std::move(leftSimp);
            return;
        }
        if (isZero(leftSimp.get()))
        {
            simplifiedNode = std::make_unique<UnaryOpNode>(UnaryOpType::Negate, std::move(rightSimp));
            return;
        }
    }

    // 4. Multiplication identities
    if (op == BinaryOpType::Mul)
    {
        if (isZero(leftSimp.get()) || isZero(rightSimp.get()))
        {
            simplifiedNode = std::make_unique<NumberNode>(0.0);
            return;
        }
        if (isOne(leftSimp.get()))
        {
            simplifiedNode = std::move(rightSimp);
            return;
        }
        if (isOne(rightSimp.get()))
        {
            simplifiedNode = std::move(leftSimp);
            return;
        }
    }

    // 5. Division identities
    if (op == BinaryOpType::Div)
    {
        if (isZero(leftSimp.get()) && !isZero(rightSimp.get()))
        {
            simplifiedNode = std::make_unique<NumberNode>(0.0);
            return;
        }
        if (isOne(rightSimp.get()))
        {
            simplifiedNode = std::move(leftSimp);
            return;
        }
    }

    // 6. Exponentiation identities
    if (op == BinaryOpType::Pow)
    {
        if (isZero(rightSimp.get()))
        {
            simplifiedNode = std::make_unique<NumberNode>(1.0);
            return;
        }
        if (isOne(rightSimp.get()))
        {
            simplifiedNode = std::move(leftSimp);
            return;
        }
        if (isZero(leftSimp.get()))
        {
            simplifiedNode = std::make_unique<NumberNode>(0.0);
            return;
        }
        if (isOne(leftSimp.get()))
        {
            simplifiedNode = std::make_unique<NumberNode>(1.0);
            return;
        }
    }

    simplifiedNode = std::make_unique<BinaryOpNode>(op, std::move(leftSimp), std::move(rightSimp));
}

void SimplifyVisitor::visit(UnaryOpNode *node)
{
    std::unique_ptr<ASTNode> opndSimp;
    if (node->operand)
    {
        node->operand->accept(this);
        opndSimp = std::move(simplifiedNode);
    }

    // Double negation: -(-x) -> x
    if (node->opType == UnaryOpType::Negate)
    {
        if (auto innerUnary = dynamic_cast<UnaryOpNode *>(opndSimp.get()))
        {
            if (innerUnary->opType == UnaryOpType::Negate)
            {
                simplifiedNode = innerUnary->operand ? innerUnary->operand->clone() : nullptr;
                return;
            }
        }
        if (auto num = dynamic_cast<NumberNode *>(opndSimp.get()))
        {
            simplifiedNode = std::make_unique<NumberNode>(-num->value);
            return;
        }
    }

    simplifiedNode = std::make_unique<UnaryOpNode>(node->opType, std::move(opndSimp));
}

void SimplifyVisitor::visit(FunctionNode *node)
{
    std::vector<std::unique_ptr<ASTNode>> simpArgs;
    simpArgs.reserve(node->arguments.size());

    for (const auto &arg : node->arguments)
    {
        if (arg)
        {
            arg->accept(this);
            simpArgs.push_back(std::move(simplifiedNode));
        }
    }

    // Check single-argument trigonometric and math function identities
    if (simpArgs.size() == 1)
    {
        if (auto num = dynamic_cast<NumberNode *>(simpArgs[0].get()))
        {
            double v = num->value;
            const std::string &fn = node->name;
            if (fn == "sin")
            {
                simplifiedNode = std::make_unique<NumberNode>(std::sin(v));
                return;
            }
            if (fn == "cos")
            {
                simplifiedNode = std::make_unique<NumberNode>(std::cos(v));
                return;
            }
            if (fn == "tan")
            {
                simplifiedNode = std::make_unique<NumberNode>(std::tan(v));
                return;
            }
            if (fn == "sinh")
            {
                simplifiedNode = std::make_unique<NumberNode>(std::sinh(v));
                return;
            }
            if (fn == "cosh")
            {
                simplifiedNode = std::make_unique<NumberNode>(std::cosh(v));
                return;
            }
            if (fn == "tanh")
            {
                simplifiedNode = std::make_unique<NumberNode>(std::tanh(v));
                return;
            }
            if (fn == "exp")
            {
                simplifiedNode = std::make_unique<NumberNode>(std::exp(v));
                return;
            }
            if (fn == "log" && v > 0.0)
            {
                simplifiedNode = std::make_unique<NumberNode>(std::log(v));
                return;
            }
            if (fn == "log10" && v > 0.0)
            {
                simplifiedNode = std::make_unique<NumberNode>(std::log10(v));
                return;
            }
            if (fn == "sqrt" && v >= 0.0)
            {
                simplifiedNode = std::make_unique<NumberNode>(std::sqrt(v));
                return;
            }
            if (fn == "abs")
            {
                simplifiedNode = std::make_unique<NumberNode>(std::abs(v));
                return;
            }
            if (fn == "floor")
            {
                simplifiedNode = std::make_unique<NumberNode>(std::floor(v));
                return;
            }
            if (fn == "ceil")
            {
                simplifiedNode = std::make_unique<NumberNode>(std::ceil(v));
                return;
            }
        }
    }

    simplifiedNode = std::make_unique<FunctionNode>(node->name, std::move(simpArgs));
}

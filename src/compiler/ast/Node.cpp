#include "Node.hpp"
#include "../visitors/Visitor.hpp"

BinaryOpType stringToBinaryOp(const std::string &str)
{
    if (str == "+")
        return BinaryOpType::Add;
    if (str == "-")
        return BinaryOpType::Sub;
    if (str == "*")
        return BinaryOpType::Mul;
    if (str == "/")
        return BinaryOpType::Div;
    if (str == "^")
        return BinaryOpType::Pow;
    return BinaryOpType::Unknown;
}

UnaryOpType stringToUnaryOp(const std::string &str)
{
    if (str == "-")
        return UnaryOpType::Negate;
    return UnaryOpType::Unknown;
}

std::string binaryOpToString(BinaryOpType type)
{
    switch (type)
    {
    case BinaryOpType::Add:
        return "+";
    case BinaryOpType::Sub:
        return "-";
    case BinaryOpType::Mul:
        return "*";
    case BinaryOpType::Div:
        return "/";
    case BinaryOpType::Pow:
        return "^";
    default:
        return "";
    }
}

std::string unaryOpToString(UnaryOpType type)
{
    switch (type)
    {
    case UnaryOpType::Negate:
        return "-";
    default:
        return "";
    }
}

void NumberNode::accept(Visitor *visitor)
{
    visitor->visit(this);
}

std::unique_ptr<ASTNode> NumberNode::clone() const
{
    return std::make_unique<NumberNode>(value);
}

void VariableNode::accept(Visitor *visitor)
{
    visitor->visit(this);
}

std::unique_ptr<ASTNode> VariableNode::clone() const
{
    return std::make_unique<VariableNode>(name);
}

void ConstantNode::accept(Visitor *visitor)
{
    visitor->visit(this);
}

std::unique_ptr<ASTNode> ConstantNode::clone() const
{
    return std::make_unique<ConstantNode>(name, value);
}

BinaryOpNode::BinaryOpNode(std::string operation, std::unique_ptr<ASTNode> l, std::unique_ptr<ASTNode> r)
    : opType(stringToBinaryOp(operation)), left(std::move(l)), right(std::move(r)) {}

BinaryOpNode::BinaryOpNode(BinaryOpType type, std::unique_ptr<ASTNode> l, std::unique_ptr<ASTNode> r)
    : opType(type), left(std::move(l)), right(std::move(r)) {}

void BinaryOpNode::accept(Visitor *visitor)
{
    visitor->visit(this);
}

std::unique_ptr<ASTNode> BinaryOpNode::clone() const
{
    return std::make_unique<BinaryOpNode>(
        opType,
        left ? left->clone() : nullptr,
        right ? right->clone() : nullptr);
}

UnaryOpNode::UnaryOpNode(std::string operation, std::unique_ptr<ASTNode> opnd)
    : opType(stringToUnaryOp(operation)), operand(std::move(opnd)) {}

UnaryOpNode::UnaryOpNode(UnaryOpType type, std::unique_ptr<ASTNode> opnd)
    : opType(type), operand(std::move(opnd)) {}

void UnaryOpNode::accept(Visitor *visitor)
{
    visitor->visit(this);
}

std::unique_ptr<ASTNode> UnaryOpNode::clone() const
{
    return std::make_unique<UnaryOpNode>(
        opType,
        operand ? operand->clone() : nullptr);
}

void FunctionNode::accept(Visitor *visitor)
{
    visitor->visit(this);
}

std::unique_ptr<ASTNode> FunctionNode::clone() const
{
    std::vector<std::unique_ptr<ASTNode>> clonedArgs;
    clonedArgs.reserve(arguments.size());
    for (const auto &arg : arguments)
    {
        clonedArgs.push_back(arg ? arg->clone() : nullptr);
    }
    return std::make_unique<FunctionNode>(name, std::move(clonedArgs));
}

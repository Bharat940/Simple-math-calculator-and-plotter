#pragma once

#include <memory>
#include <string>
#include <vector>
#include <cstdint>

class Visitor;

enum class BinaryOpType : uint8_t
{
    Add,
    Sub,
    Mul,
    Div,
    Pow,
    Unknown
};

enum class UnaryOpType : uint8_t
{
    Negate,
    Unknown
};

BinaryOpType stringToBinaryOp(const std::string &str);
UnaryOpType stringToUnaryOp(const std::string &str);
std::string binaryOpToString(BinaryOpType type);
std::string unaryOpToString(UnaryOpType type);

class ASTNode
{
public:
    virtual ~ASTNode() = default;
    virtual void accept(Visitor *visitor) = 0;
    virtual std::unique_ptr<ASTNode> clone() const = 0;
};

class NumberNode : public ASTNode
{
public:
    double value;
    explicit NumberNode(double val) : value(val) {}
    void accept(Visitor *visitor) override;
    std::unique_ptr<ASTNode> clone() const override;
};

class VariableNode : public ASTNode
{
public:
    std::string name;
    explicit VariableNode(std::string varName) : name(std::move(varName)) {}
    void accept(Visitor *visitor) override;
    std::unique_ptr<ASTNode> clone() const override;
};

class ConstantNode : public ASTNode
{
public:
    std::string name;
    double value;
    ConstantNode(std::string cName, double cVal) : name(std::move(cName)), value(cVal) {}
    void accept(Visitor *visitor) override;
    std::unique_ptr<ASTNode> clone() const override;
};

class BinaryOpNode : public ASTNode
{
public:
    BinaryOpType opType = BinaryOpType::Unknown;
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;

    BinaryOpNode(std::string operation, std::unique_ptr<ASTNode> l, std::unique_ptr<ASTNode> r);
    BinaryOpNode(BinaryOpType type, std::unique_ptr<ASTNode> l, std::unique_ptr<ASTNode> r);
    std::string getOpString() const { return binaryOpToString(opType); }
    void accept(Visitor *visitor) override;
    std::unique_ptr<ASTNode> clone() const override;
};

class UnaryOpNode : public ASTNode
{
public:
    UnaryOpType opType = UnaryOpType::Unknown;
    std::unique_ptr<ASTNode> operand;

    UnaryOpNode(std::string operation, std::unique_ptr<ASTNode> opnd);
    UnaryOpNode(UnaryOpType type, std::unique_ptr<ASTNode> opnd);
    std::string getOpString() const { return unaryOpToString(opType); }
    void accept(Visitor *visitor) override;
    std::unique_ptr<ASTNode> clone() const override;
};

class FunctionNode : public ASTNode
{
public:
    std::string name;
    std::vector<std::unique_ptr<ASTNode>> arguments;
    FunctionNode(std::string fnName, std::vector<std::unique_ptr<ASTNode>> args)
        : name(std::move(fnName)), arguments(std::move(args)) {}
    void accept(Visitor *visitor) override;
    std::unique_ptr<ASTNode> clone() const override;
};

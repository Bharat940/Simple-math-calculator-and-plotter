#include "PrintVisitor.hpp"
#include <sstream>

void PrintVisitor::visit(NumberNode *node)
{
    std::ostringstream ss;
    ss << node->value;
    result = ss.str();
}

void PrintVisitor::visit(VariableNode *node)
{
    result = node->name;
}

void PrintVisitor::visit(ConstantNode *node)
{
    result = node->name;
}

void PrintVisitor::visit(BinaryOpNode *node)
{
    std::string leftStr, rightStr;
    if (node->left)
    {
        node->left->accept(this);
        leftStr = result;
    }
    if (node->right)
    {
        node->right->accept(this);
        rightStr = result;
    }
    result = "(" + leftStr + " " + node->getOpString() + " " + rightStr + ")";
}

void PrintVisitor::visit(UnaryOpNode *node)
{
    std::string opndStr;
    if (node->operand)
    {
        node->operand->accept(this);
        opndStr = result;
    }
    result = "(" + node->getOpString() + opndStr + ")";
}

void PrintVisitor::visit(FunctionNode *node)
{
    std::string str = node->name + "(";
    for (size_t i = 0; i < node->arguments.size(); ++i)
    {
        if (node->arguments[i])
        {
            node->arguments[i]->accept(this);
            str += result;
            if (i + 1 < node->arguments.size())
                str += ", ";
        }
    }
    str += ")";
    result = str;
}

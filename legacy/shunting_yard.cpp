#include "shunting_yard.h"
#include <stack>
#include <stdexcept>

static int getPrecedence(const std::string &op)
{
    if (op == "+" || op == "-")
        return 1;
    if (op == "*" || op == "/")
        return 2;
    if (op == "^")
        return 3;
    return 0;
}

static bool isRightAssociative(const std::string &op)
{
    return op == "^";
}

std::vector<Token> legacyToPostfix(const std::vector<Token> &infix)
{
    std::vector<Token> output;
    std::stack<Token> opStack;

    for (const auto &token : infix)
    {
        switch (token.type)
        {
        case TokenType::Number:
        case TokenType::Variable:
        case TokenType::Constant:
            output.push_back(token);
            break;
        case TokenType::Function:
            opStack.push(token);
            break;
        case TokenType::Operator:
        {
            while (!opStack.empty())
            {
                const auto &top = opStack.top();
                if (top.type == TokenType::Operator)
                {
                    int p1 = getPrecedence(token.value);
                    int p2 = getPrecedence(top.value);
                    if ((!isRightAssociative(token.value) && p1 <= p2) ||
                        (isRightAssociative(token.value) && p1 < p2))
                    {
                        output.push_back(top);
                        opStack.pop();
                        continue;
                    }
                }
                break;
            }
            opStack.push(token);
            break;
        }
        case TokenType::LeftParen:
            opStack.push(token);
            break;
        case TokenType::RightParen:
        {
            bool foundLParen = false;
            while (!opStack.empty())
            {
                if (opStack.top().type == TokenType::LeftParen)
                {
                    foundLParen = true;
                    opStack.pop();
                    break;
                }
                output.push_back(opStack.top());
                opStack.pop();
            }
            if (!foundLParen)
                throw std::runtime_error("Mismatched parentheses");
            if (!opStack.empty() && opStack.top().type == TokenType::Function)
            {
                output.push_back(opStack.top());
                opStack.pop();
            }
            break;
        }
        default:
            break;
        }
    }
    while (!opStack.empty())
    {
        if (opStack.top().type == TokenType::LeftParen || opStack.top().type == TokenType::RightParen)
        {
            throw std::runtime_error("Mismatched parentheses");
        }
        output.push_back(opStack.top());
        opStack.pop();
    }
    return output;
}

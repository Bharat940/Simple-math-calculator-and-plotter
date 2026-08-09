#include "PrattParser.hpp"
#include "../../core/FunctionRegistry.hpp"
#include <stdexcept>

PrattParser::PrattParser(std::vector<Token> tokList, DiagnosticsReporter &diag)
    : tokens(std::move(tokList)), diagnostics(diag) {}

const Token &PrattParser::peek() const
{
    if (pos < tokens.size())
        return tokens[pos];
    static Token endToken{TokenType::Operator, "", 0};
    return endToken;
}

Token PrattParser::advance()
{
    if (pos < tokens.size())
        return tokens[pos++];
    return {TokenType::Operator, "", 0};
}

bool PrattParser::check(TokenType type) const
{
    return pos < tokens.size() && peek().type == type;
}

BindingPower PrattParser::infixBindingPower(const std::string &op) const
{
    if (op == "+" || op == "-")
        return {10, 11};
    if (op == "*" || op == "/")
        return {20, 21};
    if (op == "^")
        return {40, 39}; // Right associative
    return {0, 0};
}

BindingPower PrattParser::prefixBindingPower(const std::string &op) const
{
    if (op == "-")
        return {0, 30};
    return {0, 0};
}

std::unique_ptr<ASTNode> PrattParser::parseExpression(int minBp)
{
    if (pos >= tokens.size())
        return nullptr;

    Token token = advance();
    std::unique_ptr<ASTNode> left;

    if (token.type == TokenType::Number)
    {
        left = std::make_unique<NumberNode>(std::stod(token.value));
    }
    else if (token.type == TokenType::Variable)
    {
        left = std::make_unique<VariableNode>(token.value);
    }
    else if (token.type == TokenType::Constant)
    {
        double val = 0.0;
        if (token.value == "pi")
            val = 3.14159265358979323846;
        else if (token.value == "e")
            val = 2.71828182845904523536;
        else if (token.value == "phi")
            val = 1.61803398874989484820;
        else if (token.value == "tau")
            val = 6.28318530717958647692;
        left = std::make_unique<ConstantNode>(token.value, val);
    }
    else if (token.type == TokenType::Function)
    {
        std::string fnName = token.value;
        if (!mathstudio::core::FunctionRegistry::instance().has(fnName))
        {
            diagnostics.reportError("Unknown function: " + fnName, 1, pos);
            return nullptr;
        }
        if (check(TokenType::LeftParen))
        {
            advance(); // Consume '('
            std::vector<std::unique_ptr<ASTNode>> args;
            if (!check(TokenType::RightParen))
            {
                while (true)
                {
                    args.push_back(parseExpression(0));
                    if (check(TokenType::Comma))
                    {
                        advance();
                    }
                    else
                    {
                        break;
                    }
                }
            }
            if (check(TokenType::RightParen))
            {
                advance(); // Consume ')'
            }
            else
            {
                diagnostics.reportError("Expected ')' after function arguments", 1, pos);
            }
            left = std::make_unique<FunctionNode>(fnName, std::move(args));
        }
        else
        {
            diagnostics.reportError("Expected '(' after function name", 1, pos);
        }
    }
    else if (token.type == TokenType::LeftParen)
    {
        left = parseExpression(0);
        if (check(TokenType::RightParen))
        {
            advance(); // Consume ')'
        }
        else
        {
            diagnostics.reportError("Unmatched '(' - missing closing parenthesis", 1, pos);
        }
    }
    else if (token.type == TokenType::Operator && token.value == "-")
    {
        auto bp = prefixBindingPower("-");
        auto operand = parseExpression(bp.right);
        left = std::make_unique<UnaryOpNode>("-", std::move(operand));
    }
    else
    {
        diagnostics.reportError("Unexpected token: " + token.value, 1, pos);
        return nullptr;
    }

    while (pos < tokens.size())
    {
        Token opToken = peek();
        if (opToken.type != TokenType::Operator)
        {
            break;
        }

        auto bp = infixBindingPower(opToken.value);
        if (bp.left < minBp)
        {
            break;
        }

        advance(); // Consume operator token
        auto right = parseExpression(bp.right);
        left = std::make_unique<BinaryOpNode>(opToken.value, std::move(left), std::move(right));
    }

    return left;
}

std::unique_ptr<ASTNode> PrattParser::parse()
{
    if (tokens.empty())
        return nullptr;
    return parseExpression(0);
}

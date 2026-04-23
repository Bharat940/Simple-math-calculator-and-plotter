#include "tokenizer.h"
#include "constants_registry.h"

#include <cctype>    // std::isdigit, std::isalpha, std::isspace
#include <stdexcept> // std::runtime_error
#include <string>
#include <vector>

static bool needsImplicitMul(TokenType left, TokenType right)
{
    bool leftOk =
        left == TokenType::Number ||
        left == TokenType::Variable ||
        left == TokenType::Constant ||
        left == TokenType::RightParen;

    bool rightOk =
        right == TokenType::Variable ||
        right == TokenType::Constant ||
        right == TokenType::Function ||
        right == TokenType::LeftParen;

    return leftOk && rightOk;
}

/*
    Tokenizes a mathematical expression string into a sequence of tokens.
*/
std::vector<Token> tokenize(const std::string &expr)
{
    std::vector<Token> tokens;
    size_t i = 0;

    while (i < expr.length())
    {
        // -------------------------------------------------
        // 1. Ignore whitespace
        // -------------------------------------------------
        if (std::isspace(expr[i]))
        {
            ++i;
            continue;
        }

        // -------------------------------------------------
        // 2. Number
        // -------------------------------------------------
        if (std::isdigit(expr[i]) || expr[i] == '.')
        {
            std::string number;

            while (i < expr.length() &&
                   (std::isdigit(expr[i]) || expr[i] == '.'))
            {
                number += expr[i];
                ++i;
            }

            Token newTok{TokenType::Number, number};

            if (!tokens.empty() &&
                needsImplicitMul(tokens.back().type, newTok.type))
            {
                tokens.push_back({TokenType::Operator, "*"});
            }

            tokens.push_back(newTok);
            continue;
        }

        // -------------------------------------------------
        // 3. Variable / Constant / Function
        // -------------------------------------------------
        if (std::isalpha(expr[i]))
        {
            std::string name;

            while (i < expr.length() && std::isalpha(expr[i]))
            {
                name += expr[i];
                ++i;
            }

            const auto &constants = getConstantRegistry();
            Token newTok;

            if (name == "x")
            {
                newTok = {TokenType::Variable, name};
            }
            else if (constants.count(name))
            {
                newTok = {TokenType::Constant, name};
            }
            else
            {
                newTok = {TokenType::Function, name};
            }

            if (!tokens.empty() &&
                needsImplicitMul(tokens.back().type, newTok.type))
            {
                tokens.push_back({TokenType::Operator, "*"});
            }

            tokens.push_back(newTok);
            continue;
        }

        // -------------------------------------------------
        // 4. Comma
        // -------------------------------------------------
        if (expr[i] == ',')
        {
            tokens.push_back({TokenType::Comma, ","});
            ++i;
            continue;
        }

        // -------------------------------------------------
        // 5. Operators
        // -------------------------------------------------
        if (expr[i] == '+' || expr[i] == '-' ||
            expr[i] == '*' || expr[i] == '/' ||
            expr[i] == '^')
        {
            tokens.push_back(
                {TokenType::Operator, std::string(1, expr[i])});
            ++i;
            continue;
        }

        // -------------------------------------------------
        // 6. Parentheses
        // -------------------------------------------------
        if (expr[i] == '(')
        {
            Token newTok{TokenType::LeftParen, "("};

            if (!tokens.empty() &&
                needsImplicitMul(tokens.back().type, newTok.type))
            {
                tokens.push_back({TokenType::Operator, "*"});
            }

            tokens.push_back(newTok);
            ++i;
            continue;
        }

        if (expr[i] == ')')
        {
            tokens.push_back({TokenType::RightParen, ")"});
            ++i;
            continue;
        }

        // -------------------------------------------------
        // 7. Invalid character
        // -------------------------------------------------
        throw std::runtime_error(
            std::string("Invalid character in expression: '") +
            expr[i] + "'");
    }

    return tokens;
}

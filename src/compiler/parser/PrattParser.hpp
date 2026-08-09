#pragma once

#include "../../math/tokenizer.h"
#include "../ast/Node.hpp"
#include "../diagnostics/Diagnostics.hpp"
#include <memory>
#include <vector>

struct BindingPower
{
    int left;
    int right;
};

class PrattParser
{
private:
    std::vector<Token> tokens;
    size_t pos = 0;
    DiagnosticsReporter &diagnostics;

    const Token &peek() const;
    Token advance();
    bool check(TokenType type) const;

    BindingPower infixBindingPower(const std::string &op) const;
    BindingPower prefixBindingPower(const std::string &op) const;

    std::unique_ptr<ASTNode> parseExpression(int minBp = 0);

public:
    PrattParser(std::vector<Token> tokList, DiagnosticsReporter &diag);
    std::unique_ptr<ASTNode> parse();
};

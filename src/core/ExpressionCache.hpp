#pragma once

#include "../compiler/ast/Node.hpp"
#include <unordered_map>
#include <string>
#include <memory>

class ExpressionCache
{
private:
    std::unordered_map<std::string, std::unique_ptr<ASTNode>> cache;

public:
    ExpressionCache() = default;
    static ExpressionCache &instance();
    std::unique_ptr<ASTNode> get(const std::string &exprStr);
    void store(const std::string &exprStr, const ASTNode *ast);
    void clear();
};

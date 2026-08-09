#include "ExpressionCache.hpp"

ExpressionCache &ExpressionCache::instance()
{
    static ExpressionCache instance;
    return instance;
}

std::unique_ptr<ASTNode> ExpressionCache::get(const std::string &exprStr)
{
    auto it = cache.find(exprStr);
    if (it != cache.end() && it->second)
    {
        return it->second->clone();
    }
    return nullptr;
}

void ExpressionCache::store(const std::string &exprStr, const ASTNode *ast)
{
    if (ast)
    {
        cache[exprStr] = ast->clone();
    }
}

void ExpressionCache::clear()
{
    cache.clear();
}

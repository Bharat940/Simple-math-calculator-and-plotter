#include "ExpressionCache.hpp"
#include "core/performance/FrameProfiler.hpp"

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
        ::mathstudio::core::performance::FrameProfiler::instance().recordCacheHit();
        return it->second->clone();
    }
    ::mathstudio::core::performance::FrameProfiler::instance().recordCacheMiss();
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

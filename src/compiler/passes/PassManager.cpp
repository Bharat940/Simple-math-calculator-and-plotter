#include "PassManager.hpp"

void PassManager::addPass(std::unique_ptr<ASTPass> pass)
{
    if (pass)
    {
        passes.push_back(std::move(pass));
    }
}

std::unique_ptr<ASTNode> PassManager::run(std::unique_ptr<ASTNode> root)
{
    std::unique_ptr<ASTNode> current = std::move(root);
    for (auto &pass : passes)
    {
        if (current)
        {
            current = pass->run(std::move(current));
        }
    }
    return current;
}

#pragma once

#include "../ast/Node.hpp"
#include <memory>
#include <vector>

class ASTPass
{
public:
    virtual ~ASTPass() = default;
    virtual std::unique_ptr<ASTNode> run(std::unique_ptr<ASTNode> node) = 0;
};

class PassManager
{
private:
    std::vector<std::unique_ptr<ASTPass>> passes;

public:
    void addPass(std::unique_ptr<ASTPass> pass);
    std::unique_ptr<ASTNode> run(std::unique_ptr<ASTNode> root);
};

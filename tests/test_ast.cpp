#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <cassert>

#include "../src/core/MathValue.hpp"
#include "../src/core/VariableStore.hpp"
#include "../src/core/FunctionRegistry.hpp"
#include "../src/core/EvaluationContext.hpp"
#include "../src/core/ExpressionCache.hpp"
#include "../src/compiler/diagnostics/Diagnostics.hpp"
#include "../src/compiler/ast/Node.hpp"
#include "../src/compiler/parser/PrattParser.hpp"
#include "../src/compiler/visitors/EvalVisitor.hpp"
#include "../src/compiler/visitors/PrintVisitor.hpp"
#include "../src/compiler/passes/ConstantFoldVisitor.hpp"
#include "../src/compiler/passes/SimplifyVisitor.hpp"
#include "../src/math/expression.h"

using namespace mathstudio::core;

int main()
{
    std::cout << "Step 0: Getting FunctionRegistry instance...\n"
              << std::flush;
    const auto &fns = FunctionRegistry::instance();
    std::cout << "Step 0 passed, functions count: " << fns.getAll().size() << "\n"
              << std::flush;

    std::cout << "Step 1: MathValue real\n"
              << std::flush;
    auto r = MathValue::real(42.0);
    std::cout << "Step 1 passed: " << r.getReal() << "\n"
              << std::flush;

    std::cout << "Step 2: VariableStore O(1) slots\n"
              << std::flush;
    VariableStore vars;
    vars.set("x", MathValue::real(10.0));
    assert(vars.getX().getReal() == 10.0);
    assert(vars.get("x").getReal() == 10.0);
    std::cout << "Step 2 passed, O(1) vars check OK\n"
              << std::flush;

    std::cout << "Step 3: Diagnostics & Context\n"
              << std::flush;
    DiagnosticsReporter diag;
    EvaluationContext ctx(vars, fns, diag);
    std::cout << "Step 3 passed\n"
              << std::flush;

    std::cout << "Step 4: Tokenize\n"
              << std::flush;
    std::string exprStr = "2 + 3 * 4";
    auto tokens = tokenize(exprStr);
    std::cout << "Step 4 passed, tokens count: " << tokens.size() << "\n"
              << std::flush;

    std::cout << "Step 5: PrattParser\n"
              << std::flush;
    PrattParser parser(tokens, diag);
    auto ast = parser.parse();
    std::cout << "Step 5 passed, ast: " << (ast ? "valid" : "null") << "\n"
              << std::flush;

    std::cout << "Step 6: EvalVisitor construction\n"
              << std::flush;
    EvalVisitor visitor(ctx);
    std::cout << "Step 6 passed\n"
              << std::flush;

    std::cout << "Step 7: AST accept EvalVisitor\n"
              << std::flush;
    if (ast)
    {
        ast->accept(&visitor);
        std::cout << "Step 7 passed, result: " << visitor.getResult().getReal() << "\n"
                  << std::flush;
    }

    std::cout << "Step 8: ConstantFoldVisitor\n"
              << std::flush;
    ConstantFoldVisitor foldVisitor;
    auto folded = foldVisitor.run(std::move(ast));
    std::cout << "Step 8 passed, folded: " << (folded ? "valid" : "null") << "\n"
              << std::flush;

    std::cout << "Step 9: SimplifyVisitor algebraic identities\n"
              << std::flush;
    SimplifyVisitor simp;

    // Test x + 0 -> x
    auto tokens1 = tokenize("x + 0");
    PrattParser p1(tokens1, diag);
    auto simplified1 = simp.run(p1.parse());
    assert(dynamic_cast<VariableNode *>(simplified1.get()) != nullptr);

    // Test x * 1 -> x
    auto tokens2 = tokenize("x * 1");
    PrattParser p2(tokens2, diag);
    auto simplified2 = simp.run(p2.parse());
    assert(dynamic_cast<VariableNode *>(simplified2.get()) != nullptr);

    // Test x * 0 -> 0
    auto tokens3 = tokenize("x * 0");
    PrattParser p3(tokens3, diag);
    auto simplified3 = simp.run(p3.parse());
    auto num3 = dynamic_cast<NumberNode *>(simplified3.get());
    assert(num3 && num3->value == 0.0);
    (void)num3;

    // Test sin(0) -> 0
    auto tokens4 = tokenize("sin(0)");
    PrattParser p4(tokens4, diag);
    auto simplified4 = simp.run(p4.parse());
    auto num4 = dynamic_cast<NumberNode *>(simplified4.get());
    assert(num4 && num4->value == 0.0);
    (void)num4;

    std::cout << "Step 9 passed, all SimplifyVisitor rules verified!\n"
              << std::flush;

    std::cout << "ALL STEPS COMPLETED!\n"
              << std::flush;
    return 0;
}

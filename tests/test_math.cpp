// tests/test_math.cpp
// Lightweight unit tests for the math pipeline -- no external dependencies.
//
// Build:  cmake --build build --target tests
// Run:    ./build/tests       (or .\build\Debug\tests.exe on Windows)

#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <limits>

#include "../src/math/tokenizer.h"
#include "../src/math/parser.h"
#include "../src/math/evaluator.h"
#include "../src/math/expression.h"
#include "../src/math/solver.h"
#include "../src/math/numerical.h"
#include "../src/math/constants.h"
#include "../src/math/functions.h"
#include "../src/math/constants_registry.h"

// Minimal test framework

static int g_total   = 0;
static int g_passed  = 0;
static int g_failed  = 0;
static std::string g_currentSuite;

#define TEST_SUITE(name) \
    do { g_currentSuite = name; } while (0)

#define CHECK(condition, description)                                          \
    do {                                                                       \
        ++g_total;                                                             \
        if (condition) {                                                       \
            ++g_passed;                                                        \
        } else {                                                               \
            ++g_failed;                                                        \
            std::cerr << "  FAIL [" << g_currentSuite << "] "                  \
                      << description << "  (" << __FILE__ << ":"               \
                      << __LINE__ << ")" << std::endl;                         \
        }                                                                      \
    } while (0)

#define CHECK_APPROX(actual, expected, eps, description)                       \
    CHECK(std::abs((actual) - (expected)) < (eps), description)

#define CHECK_THROWS(expression, description)                                  \
    do {                                                                       \
        ++g_total;                                                             \
        bool threw = false;                                                    \
        try { expression; } catch (...) { threw = true; }                      \
        if (threw) {                                                           \
            ++g_passed;                                                        \
        } else {                                                               \
            ++g_failed;                                                        \
            std::cerr << "  FAIL [" << g_currentSuite << "] "                  \
                      << description << " (expected exception)"                \
                      << "  (" << __FILE__ << ":" << __LINE__ << ")"           \
                      << std::endl;                                            \
        }                                                                      \
    } while (0)

// Helpers

static double evalExpr(const std::string &expr, double x = 0.0)
{
    try
    {
        Expression e(expr);
        return e.eval(x);
    }
    catch (...)
    {
        return std::numeric_limits<double>::quiet_NaN();
    }
}

static EvalResult evalExprSafe(const std::string &expr, double x = 0.0)
{
    try
    {
        Expression e(expr);
        return e.evalSafe(x);
    }
    catch (const std::exception &ex)
    {
        return EvalResult::failure(ex.what());
    }
}

// Test suites

static void testTokenizer()
{
    TEST_SUITE("Tokenizer");

    // Basic number
    auto tokens = tokenize("42");
    CHECK(tokens.size() == 1, "single number token count");
    CHECK(tokens[0].type == TokenType::Number, "single number type");
    CHECK(tokens[0].value == "42", "single number value");

    // Variable
    tokens = tokenize("x");
    CHECK(tokens.size() == 1, "variable token count");
    CHECK(tokens[0].type == TokenType::Variable, "variable type");

    // Simple expression
    tokens = tokenize("x+1");
    CHECK(tokens.size() == 3, "x+1 token count");
    CHECK(tokens[1].type == TokenType::Operator, "plus is operator");

    // Implicit multiplication: 2x -> 2 * x
    tokens = tokenize("2x");
    CHECK(tokens.size() == 3, "2x implicit mul token count");
    CHECK(tokens[1].type == TokenType::Operator, "2x implicit mul operator");
    CHECK(tokens[1].value == "*", "2x implicit mul is *");

    // Implicit multiplication: x(x+1)
    tokens = tokenize("x(x+1)");
    CHECK(tokens.size() == 7, "x(x+1) token count");
    CHECK(tokens[1].type == TokenType::Operator, "x( implicit mul");

    // Function recognition
    tokens = tokenize("sin(x)");
    CHECK(tokens[0].type == TokenType::Function, "sin is function token");
    CHECK(tokens[0].value == "sin", "sin function name");

    // Constant recognition
    tokens = tokenize("pi");
    CHECK(tokens[0].type == TokenType::Constant, "pi is constant token");

    // Invalid character
    CHECK_THROWS(tokenize("x & y"), "invalid character throws");

    // Decimal number
    tokens = tokenize("3.14");
    CHECK(tokens.size() == 1, "decimal number token count");
    CHECK(tokens[0].value == "3.14", "decimal number value");

    // Multiple decimal points rejected
    CHECK_THROWS(tokenize("1.2.3"), "multiple decimal points throws during tokenization");

    // Whitespace handling
    tokens = tokenize("  x  +  1  ");
    CHECK(tokens.size() == 3, "whitespace ignored token count");
}

static void testParser()
{
    TEST_SUITE("Parser");

    // Simple postfix conversion: 1 + 2 -> 1 2 +
    auto tokens = tokenize("1+2");
    auto postfix = toPostfix(tokens);
    CHECK(postfix.size() == 3, "1+2 postfix length");
    CHECK(postfix[0].value == "1", "1+2 postfix[0]");
    CHECK(postfix[1].value == "2", "1+2 postfix[1]");
    CHECK(postfix[2].value == "+", "1+2 postfix[2]");

    // Operator precedence: 1 + 2 * 3 -> 1 2 3 * +
    tokens = tokenize("1+2*3");
    postfix = toPostfix(tokens);
    CHECK(postfix.size() == 5, "1+2*3 postfix length");
    CHECK(postfix[3].value == "*", "mul before add in postfix");
    CHECK(postfix[4].value == "+", "add after mul in postfix");

    // Parentheses: (1 + 2) * 3
    tokens = tokenize("(1+2)*3");
    postfix = toPostfix(tokens);
    CHECK(postfix.size() == 5, "(1+2)*3 postfix length");
    CHECK(postfix[2].value == "+", "parens force add first");
    CHECK(postfix[4].value == "*", "mul comes after parens");

    // Power right-associativity: 2^3^2 = 2^(3^2) = 2^9 = 512
    double val = evalExpr("2^3^2");
    CHECK_APPROX(val, 512.0, 1e-9, "2^3^2 right-associative = 512");

    // Unary minus
    val = evalExpr("-3+5");
    CHECK_APPROX(val, 2.0, 1e-9, "-3+5 = 2");

    // Mismatched parentheses
    CHECK_THROWS(Expression("(x+1"), "unclosed left paren throws");
    CHECK_THROWS(Expression("((x+1)"), "nested unclosed paren throws");
}

static void testEvaluator()
{
    TEST_SUITE("Evaluator");

    // Basic arithmetic
    CHECK_APPROX(evalExpr("2+3"), 5.0, 1e-9, "2+3 = 5");
    CHECK_APPROX(evalExpr("10-4"), 6.0, 1e-9, "10-4 = 6");
    CHECK_APPROX(evalExpr("3*7"), 21.0, 1e-9, "3*7 = 21");
    CHECK_APPROX(evalExpr("15/4"), 3.75, 1e-9, "15/4 = 3.75");
    CHECK_APPROX(evalExpr("2^10"), 1024.0, 1e-9, "2^10 = 1024");

    // Order of operations
    CHECK_APPROX(evalExpr("2+3*4"), 14.0, 1e-9, "2+3*4 = 14");
    CHECK_APPROX(evalExpr("(2+3)*4"), 20.0, 1e-9, "(2+3)*4 = 20");

    // Variable evaluation
    Expression xsq("x^2");
    CHECK_APPROX(xsq.eval(3.0), 9.0, 1e-9, "x^2 at x=3 is 9");
    CHECK_APPROX(xsq.eval(-5.0), 25.0, 1e-9, "x^2 at x=-5 is 25");
    CHECK_APPROX(xsq.eval(0.0), 0.0, 1e-9, "x^2 at x=0 is 0");

    // Nested expressions
    CHECK_APPROX(evalExpr("(2+3)*(4-1)"), 15.0, 1e-9, "(2+3)*(4-1) = 15");

    // Implicit multiplication
    Expression twoX("2x");
    CHECK_APPROX(twoX.eval(5.0), 10.0, 1e-9, "2x at x=5 is 10");

    // Division by zero returns error
    auto r = evalExprSafe("1/0");
    CHECK(!r.ok, "1/0 returns error");
}

static void testFunctions()
{
    TEST_SUITE("Functions");
    const double pi = 3.14159265358979323846;

    // Trigonometric
    CHECK_APPROX(evalExpr("sin(0)"), 0.0, 1e-9, "sin(0) = 0");
    CHECK_APPROX(evalExpr("cos(0)"), 1.0, 1e-9, "cos(0) = 1");
    CHECK_APPROX(evalExpr("tan(0)"), 0.0, 1e-9, "tan(0) = 0");

    Expression sinExpr("sin(x)");
    CHECK_APPROX(sinExpr.eval(pi / 2), 1.0, 1e-9, "sin(pi/2) = 1");
    CHECK_APPROX(sinExpr.eval(pi), 0.0, 1e-9, "sin(pi) ~ 0");

    Expression cosExpr("cos(x)");
    CHECK_APPROX(cosExpr.eval(pi), -1.0, 1e-9, "cos(pi) = -1");

    // Inverse trig
    CHECK_APPROX(evalExpr("asin(1)"), pi / 2, 1e-9, "asin(1) = pi/2");
    CHECK_APPROX(evalExpr("acos(1)"), 0.0, 1e-9, "acos(1) = 0");
    CHECK_APPROX(evalExpr("atan(0)"), 0.0, 1e-9, "atan(0) = 0");

    // Domain errors
    auto r1 = evalExprSafe("asin(2)");
    CHECK(!r1.ok, "asin(2) domain error");
    auto r2 = evalExprSafe("acos(2)");
    CHECK(!r2.ok, "acos(2) domain error");

    // Hyperbolic
    CHECK_APPROX(evalExpr("sinh(0)"), 0.0, 1e-9, "sinh(0) = 0");
    CHECK_APPROX(evalExpr("cosh(0)"), 1.0, 1e-9, "cosh(0) = 1");
    CHECK_APPROX(evalExpr("tanh(0)"), 0.0, 1e-9, "tanh(0) = 0");

    // Exponential and logarithmic
    CHECK_APPROX(evalExpr("exp(0)"), 1.0, 1e-9, "exp(0) = 1");
    CHECK_APPROX(evalExpr("exp(1)"), std::exp(1.0), 1e-9, "exp(1) = e");
    CHECK_APPROX(evalExpr("log(1)"), 0.0, 1e-9, "log(1) = 0");
    CHECK_APPROX(evalExpr("log10(100)"), 2.0, 1e-9, "log10(100) = 2");

    // Log domain error
    auto r3 = evalExprSafe("log(0)");
    CHECK(!r3.ok, "log(0) domain error");
    auto r4 = evalExprSafe("log(-1)");
    CHECK(!r4.ok, "log(-1) domain error");

    // Algebraic
    CHECK_APPROX(evalExpr("sqrt(9)"), 3.0, 1e-9, "sqrt(9) = 3");
    CHECK_APPROX(evalExpr("abs(-5)"), 5.0, 1e-9, "abs(-5) = 5");
    CHECK_APPROX(evalExpr("floor(3.7)"), 3.0, 1e-9, "floor(3.7) = 3");
    CHECK_APPROX(evalExpr("ceil(3.2)"), 4.0, 1e-9, "ceil(3.2) = 4");

    // Sqrt domain error
    auto r5 = evalExprSafe("sqrt(-1)");
    CHECK(!r5.ok, "sqrt(-1) domain error");

    // Binary functions
    CHECK_APPROX(evalExpr("pow(2,8)"), 256.0, 1e-9, "pow(2,8) = 256");
    CHECK_APPROX(evalExpr("max(3,7)"), 7.0, 1e-9, "max(3,7) = 7");
    CHECK_APPROX(evalExpr("min(3,7)"), 3.0, 1e-9, "min(3,7) = 3");

    // Log with base (2-argument form)
    {
        bool logBaseOk = false;
        try
        {
            Expression logExpr("log(8,2)");
            double val = logExpr.eval(0.0);
            logBaseOk = std::abs(val - 3.0) < 1e-6;
        }
        catch (...) {}
        CHECK(logBaseOk, "log(8,2) = 3");
    }

    // Unknown function
    CHECK_THROWS(Expression("foobar(x)"), "unknown function throws");
}

static void testConstants()
{
    TEST_SUITE("Constants");
    const double pi  = 3.14159265358979323846;
    const double e   = 2.71828182845904523536;
    const double phi = 1.61803398874989484820;

    CHECK_APPROX(evalExpr("pi"), pi, 1e-12, "pi constant");
    CHECK_APPROX(evalExpr("e"), e, 1e-12, "e constant");
    CHECK_APPROX(evalExpr("phi"), phi, 1e-12, "phi constant");

    // Constants in expressions
    CHECK_APPROX(evalExpr("2*pi"), 2 * pi, 1e-9, "2*pi");
    CHECK_APPROX(evalExpr("e^2"), e * e, 1e-9, "e^2");

    // sin(pi) should be near zero
    CHECK_APPROX(evalExpr("sin(pi)"), 0.0, 1e-9, "sin(pi) ~ 0");
}

static void testExpression()
{
    TEST_SUITE("Expression");

    // Valid expressions
    Expression e1("x^2 + 2*x + 1");
    CHECK_APPROX(e1.eval(1.0), 4.0, 1e-9, "(x+1)^2 at x=1 = 4");
    CHECK_APPROX(e1.eval(0.0), 1.0, 1e-9, "(x+1)^2 at x=0 = 1");
    CHECK_APPROX(e1.eval(-1.0), 0.0, 1e-9, "(x+1)^2 at x=-1 = 0");

    // evalSafe returns ok for valid
    auto r = e1.evalSafe(2.0);
    CHECK(r.ok, "evalSafe ok for valid expression");
    CHECK_APPROX(r.value, 9.0, 1e-9, "evalSafe value correct");

    // Complex expressions
    Expression e2("sin(x)^2 + cos(x)^2");
    CHECK_APPROX(e2.eval(0.0), 1.0, 1e-9, "sin^2+cos^2 = 1 at x=0");
    CHECK_APPROX(e2.eval(1.0), 1.0, 1e-9, "sin^2+cos^2 = 1 at x=1");
    CHECK_APPROX(e2.eval(2.5), 1.0, 1e-9, "sin^2+cos^2 = 1 at x=2.5");

    // Nested functions
    Expression e3("sin(cos(x))");
    double expected = std::sin(std::cos(1.0));
    CHECK_APPROX(e3.eval(1.0), expected, 1e-9, "sin(cos(1))");
}

static void testNumerical()
{
    TEST_SUITE("Numerical");

    // Derivative of x^2 at x=3 should be ~6
    Expression xsq("x^2");
    double d = derivative(xsq, 3.0);
    CHECK_APPROX(d, 6.0, 1e-4, "d/dx(x^2) at x=3 = 6");

    // Derivative of sin(x) at x=0 should be ~1 (cos(0))
    Expression sinx("sin(x)");
    d = derivative(sinx, 0.0);
    CHECK_APPROX(d, 1.0, 1e-4, "d/dx(sin(x)) at x=0 = 1");

    // Derivative of x^3 at x=2 should be ~12
    Expression xcube("x^3");
    d = derivative(xcube, 2.0);
    CHECK_APPROX(d, 12.0, 1e-3, "d/dx(x^3) at x=2 = 12");

    // Tangent line at x=1 for x^2: y = 2x - 1  (slope=2, intercept=-1)
    Line t = tangentAt(xsq, 1.0);
    CHECK_APPROX(t.m, 2.0, 1e-4, "tangent slope of x^2 at x=1 = 2");
    CHECK_APPROX(t.b, -1.0, 1e-4, "tangent intercept of x^2 at x=1 = -1");

    // Derivative with h ~ 0
    double nanVal = derivative(xsq, 3.0, 0.0);
    CHECK(std::isnan(nanVal), "derivative with h=0 is NaN");
}

static void testSolver()
{
    TEST_SUITE("Solver");

    // Roots of x^2 - 4 = 0  ->  x = -2, 2
    Expression e1("x^2-4");
    auto roots = findRoots(e1, -10, 10);
    CHECK(roots.size() == 2, "x^2-4 has 2 roots");
    if (roots.size() == 2)
    {
        CHECK_APPROX(roots[0], -2.0, 1e-4, "x^2-4 root 1 = -2");
        CHECK_APPROX(roots[1], 2.0, 1e-4, "x^2-4 root 2 = 2");
    }

    // Roots of sin(x) in [-pi, pi]  ->  x = -pi, 0, pi  (or just 0 depending on range)
    Expression sinx("sin(x)");
    roots = findRoots(sinx, -4, 4);
    CHECK(roots.size() >= 2, "sin(x) has at least 2 roots in [-4,4]");

    // Check 0 is among the roots
    bool hasZeroRoot = false;
    for (double r : roots)
    {
        if (std::abs(r) < 1e-4) hasZeroRoot = true;
    }
    CHECK(hasZeroRoot, "sin(x) has root at x=0");

    // Root of x^3 = 0  ->  x = 0
    Expression xcube("x^3");
    roots = findRoots(xcube, -5, 5);
    CHECK(!roots.empty(), "x^3 has at least 1 root");
    if (!roots.empty())
    {
        CHECK_APPROX(roots[0], 0.0, 1e-4, "x^3 root at 0");
    }

    // Detailed roots
    auto detailed = findRootsDetailed(e1, -10, 10);
    CHECK(detailed.size() == 2, "x^2-4 detailed has 2 roots");
    if (!detailed.empty())
    {
        CHECK(detailed[0].converged, "first root converged");
    }

    // Intersections of x^2 and 2*x+1 at x = 1+sqrt(2) and x = 1-sqrt(2)
    Expression f("x^2");
    Expression g("2*x+1");
    auto intersections = findIntersections(f, g, -10, 10);
    CHECK(intersections.size() == 2, "x^2 and 2x+1 have 2 intersections");

    // Extrema of x^3 - 3x  ->  derivative = 3x^2-3 = 0  ->  x = -1, 1
    Expression e3("x^3-3*x");
    auto ext = findExtrema(e3, -5, 5);
    CHECK(ext.size() == 2, "x^3-3x has 2 extrema");
    if (ext.size() == 2)
    {
        CHECK_APPROX(ext[0], -1.0, 1e-3, "extremum at x=-1");
        CHECK_APPROX(ext[1], 1.0, 1e-3, "extremum at x=1");
    }
}

static void testEdgeCases()
{
    TEST_SUITE("Edge Cases");

    // Empty expression (after trimming) should throw
    CHECK_THROWS(Expression(""), "empty expression throws");

    // Very large exponent -- should still evaluate (may be inf)
    Expression bigPow("2^100");
    double val = bigPow.eval(0);
    CHECK(std::isfinite(val), "2^100 is finite");
    CHECK_APPROX(val, std::pow(2.0, 100.0), 1e10, "2^100 correct");

    // Negative exponent
    CHECK_APPROX(evalExpr("2^(-1)"), 0.5, 1e-9, "2^(-1) = 0.5");

    // Chained operations
    CHECK_APPROX(evalExpr("1+2+3+4+5"), 15.0, 1e-9, "1+2+3+4+5 = 15");

    // Deeply nested parentheses
    CHECK_APPROX(evalExpr("((((x))))"), 0.0, 1e-9, "nested parens identity");

    // Expression with all operators
    // 2^2 + 3*2 - 10/2 = 4 + 6 - 5 = 5
    Expression all("x^2 + 3*x - 10/2");
    CHECK_APPROX(all.eval(2.0), 5.0, 1e-9, "x^2+3x-10/2 at x=2 = 5");

    // Invalid number format at evaluation level
    {
        auto r = evalExprSafe("1.2.3");
        CHECK(!r.ok, "evaluator rejects invalid number format 1.2.3");
    }
}

// Main

int main()
{
    std::cout << "Running unit tests..." << std::endl;
    std::cout << std::string(50, '-') << std::endl;

    testTokenizer();
    testParser();
    testEvaluator();
    testFunctions();
    testConstants();
    testExpression();
    testNumerical();
    testSolver();
    testEdgeCases();

    std::cout << std::string(50, '-') << std::endl;
    std::cout << "Results: " << g_passed << " passed, "
              << g_failed << " failed, "
              << g_total << " total" << std::endl;

    if (g_failed > 0)
    {
        std::cerr << "TESTS FAILED" << std::endl;
        return 1;
    }

    std::cout << "ALL TESTS PASSED" << std::endl;
    return 0;
}

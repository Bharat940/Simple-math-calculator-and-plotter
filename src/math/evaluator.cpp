#include "evaluator.h"
#include "result.h"
#include "constants.h"
#include "functions.h"
#include "constants_registry.h"
#include <stack>
#include <cmath>

EvalResult evalPostfix(
    const std::vector<Token> &postfix,
    double xValue)
{
    std::stack<double> st;

    for (const auto &token : postfix)
    {
        if (token.type == TokenType::Number)
        {
            st.push(std::stod(token.value));
        }
        else if (token.type == TokenType::Variable)
        {
            st.push(xValue);
        }
        else if (token.type == TokenType::Constant)
        {
            const auto &constants = getConstantRegistry();
            auto it = constants.find(token.value);

            if (it == constants.end())
            {
                return EvalResult::failure("Unknown constant");
            }

            st.push(it->second);
        }
        else if (token.type == TokenType::Operator)
        {
            if (st.size() < 2)
            {
                return EvalResult::failure("Invalid operator usage");
            }

            double b = st.top();
            st.pop();
            double a = st.top();
            st.pop();

            if (token.value == "/" && std::abs(b) < EPS_EVAL)
            {
                return EvalResult::failure("Division by zero");
            }

            double r;
            if (token.value == "+")
            {
                r = a + b;
            }
            else if (token.value == "-")
            {
                r = a - b;
            }
            else if (token.value == "*")
            {
                r = a * b;
            }
            else if (token.value == "/")
            {
                r = a / b;
            }
            else if (token.value == "^")
            {
                r = std::pow(a, b);
            }
            else
            {
                return EvalResult::failure("Unknown operator");
            }

            if (!std::isfinite(r))
            {
                return EvalResult::failure("Non-finite result");
            }

            st.push(r);
        }
        else if (token.type == TokenType::Function)
        {
            const auto &registry = getFunctionRegistry();
            auto it = registry.find(token.value);

            if (it == registry.end())
                return EvalResult::failure("Unknown function");

            const FunctionDef &def = it->second;
            int arity = token.arity;

            if (arity < def.minArity ||
                (def.maxArity != -1 && arity > def.maxArity))
            {
                return EvalResult::failure("Invalid number of arguments");
            }

            if (st.size() < static_cast<size_t>(arity))
                return EvalResult::failure("Not enough arguments");

            std::vector<double> args(arity);
            for (int i = arity - 1; i >= 0; --i)
            {
                args[i] = st.top();
                st.pop();
            }

            EvalResult r = def.fn(args);
            if (!r.ok)
                return r;

            st.push(r.value);
        }
    }

    if (st.size() != 1)
        return EvalResult::failure("Invalid expression");

    return EvalResult::success(st.top());
}

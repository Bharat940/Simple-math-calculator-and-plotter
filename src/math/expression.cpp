#include "expression.h"
#include "parser.h"
#include "evaluator.h"
#include "functions.h"
#include "constants_registry.h"
#include <stdexcept>
#include <limits>

Expression::Expression(const std::string &expr)
{
	auto tokens = tokenize(expr);
	postfix_ = toPostfix(tokens);

	auto v = validate();
	if (!v.ok)
	{
		throw std::runtime_error(v.error);
	}
}

EvalResult Expression::validate() const
{
	const auto &fnRegistry = getFunctionRegistry();
	const auto &constRegistry = getConstantRegistry();

	int stackDepth = 0;

	for (const auto &token : postfix_)
	{
		switch (token.type)
		{
		case TokenType::Number:
		case TokenType::Variable:
			stackDepth++;
			break;

		case TokenType::Constant:
		{
			if (!constRegistry.count(token.value))
			{
				return EvalResult::failure("Unknown constant: " + token.value);
			}
			stackDepth++;
			break;
		}

		case TokenType::Operator:
			if (stackDepth < 2)
			{
				return EvalResult::failure("Invalid operator usage: " + token.value);
			}
			stackDepth -= 1;
			break;

		case TokenType::Function:
		{
			auto it = fnRegistry.find(token.value);
			if (it == fnRegistry.end())
			{
				return EvalResult::failure("Unknown function: " + token.value);
			}

			const FunctionDef &def = it->second;

			if (token.arity <= 0)
			{
				return EvalResult::failure("Function '" + token.value + "' requires arguments");
			}

			if (token.arity < def.minArity ||
				(def.maxArity != -1 && token.arity > def.maxArity))
			{
				return EvalResult::failure("Invalid number of arguments for function '" + token.value + "'");
			}

			if (stackDepth < token.arity)
			{
				return EvalResult::failure("Not enough arguments for function '" + token.value + "'");
			}

			stackDepth -= (token.arity - 1);
			break;
		}

		default:
			return EvalResult::failure("Invalid token in expression");
		}
	}

	if (stackDepth != 1)
	{
		return EvalResult::failure("Invalid expression structure");
	}

	return EvalResult::success(0.0);
}

EvalResult Expression::evalSafe(double x) const
{
	return evalPostfix(postfix_, x);
}

double Expression::eval(double x) const
{
	auto r = evalSafe(x);
	return r.ok ? r.value : std::numeric_limits<double>::quiet_NaN();
}

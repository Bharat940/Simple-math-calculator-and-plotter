#include "parser.h"
#include <vector>
#include <stdexcept>
#include "tokenizer.h"

int precedence(const std::string &op)
{
	if (op == "+" || op == "-")
		return 1;
	if (op == "*" || op == "/")
		return 2;
	if (op == "^")
		return 3;
	return 0;
}

std::vector<Token> toPostfix(const std::vector<Token> &infix)
{
	std::vector<Token> output;
	std::vector<Token> stack;
	std::vector<int> argCount;

	TokenType prevType;
	bool hasPrev = false;

	for (const auto &token : infix)
	{
		// Values
		if (token.type == TokenType::Number ||
			token.type == TokenType::Variable ||
			token.type == TokenType::Constant)
		{
			output.push_back(token);
		}

		// Function
		else if (token.type == TokenType::Function)
		{
			stack.push_back(token);
			argCount.push_back(1);
		}

		// Comma
		else if (token.type == TokenType::Comma)
		{
			while (!stack.empty() &&
				   stack.back().type != TokenType::LeftParen)
			{
				output.push_back(stack.back());
				stack.pop_back();
			}

			if (argCount.empty())
				throw std::runtime_error("Comma outside function");

			argCount.back()++;
		}

		// Operator
		else if (token.type == TokenType::Operator)
		{
			bool isUnaryMinus =
				(token.value == "-") &&
				(!hasPrev ||
				 prevType == TokenType::Operator ||
				 prevType == TokenType::LeftParen ||
				 prevType == TokenType::Function);

			if (isUnaryMinus)
			{
				output.push_back({TokenType::Number, "0"});
			}

			while (!stack.empty() &&
				   stack.back().type == TokenType::Operator &&
				   (precedence(stack.back().value) > precedence(token.value) ||
					(precedence(stack.back().value) == precedence(token.value) &&
					 token.value != "^")))
			{
				output.push_back(stack.back());
				stack.pop_back();
			}

			stack.push_back(token);
		}

		// LeftParen
		else if (token.type == TokenType::LeftParen)
		{
			stack.push_back(token);
		}

		// RightParen
		else if (token.type == TokenType::RightParen)
		{
			if (!argCount.empty() && argCount.back() == 1 &&
				prevType == TokenType::LeftParen)
			{
				throw std::runtime_error("Empty function argument list");
			}

			while (!stack.empty() &&
				   stack.back().type != TokenType::LeftParen)
			{
				output.push_back(stack.back());
				stack.pop_back();
			}

			if (!stack.empty() &&
				stack.back().type == TokenType::LeftParen)
			{
				stack.pop_back();
			}

			if (!stack.empty() &&
				stack.back().type == TokenType::Function)
			{
				Token fn = stack.back();
				stack.pop_back();

				fn.arity = argCount.back();
				argCount.pop_back();

				output.push_back(fn);
			}
		}

		prevType = token.type;
		hasPrev = true;
	}

	while (!stack.empty())
	{
		if (stack.back().type == TokenType::LeftParen ||
			stack.back().type == TokenType::RightParen)
		{
			throw std::runtime_error("Mismatched parentheses");
		}

		output.push_back(stack.back());
		stack.pop_back();
	}

	return output;
}

#pragma once
#include <string>
#include <vector>

enum class TokenType
{
	Number,
	Variable,
	Constant,
	Operator,
	Function,
	Comma,
	LeftParen,
	RightParen
};

struct Token
{
	TokenType type;
	std::string value;
	int arity = 0;
};

std::vector<Token> tokenize(const std::string &expression);

#pragma once
#include <string>

struct EvalResult
{
	bool ok;
	double value;
	std::string error;

	static EvalResult success(double v)
	{
		return {true, v, ""};
	}

	static EvalResult failure(const std::string &msg)
	{
		return {false, 0.0, msg};
	}
};

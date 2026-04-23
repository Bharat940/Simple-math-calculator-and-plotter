#include "constants_registry.h"

static std::unordered_map<std::string, double> constants {
	{"pi", 3.14159265358979323846},
	{"e", 2.71828182845904523536},
	{"phi", 1.61803398874989484820}};

const std::unordered_map<std::string, double> &getConstantRegistry()
{
	return constants;
}

#pragma once
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>
#include "result.h"

struct FunctionDef
{
    int minArity; // minimum accepted arguments
    int maxArity; // maximum accepted arguments (-1 = unlimited)

    std::function<EvalResult(const std::vector<double> &)> fn;
};

const std::unordered_map<std::string, FunctionDef> &getFunctionRegistry();

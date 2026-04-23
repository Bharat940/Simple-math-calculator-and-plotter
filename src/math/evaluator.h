#pragma once
#include <vector>
#include "tokenizer.h"
#include "result.h"

// Pure postfix evaluator
EvalResult evalPostfix(const std::vector<Token> &postfix, double xValue);

#pragma once
#include <vector>
#include "../src/math/tokenizer.h"

// Deprecated RPN shunting-yard algorithm retained for benchmark comparisons
[[deprecated("Use PrattParser in src/compiler/parser/PrattParser.hpp instead")]]
std::vector<Token> legacyToPostfix(const std::vector<Token> &infix);

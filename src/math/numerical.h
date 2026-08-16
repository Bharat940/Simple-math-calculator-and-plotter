#pragma once

#include "expression.h"
#include "geometry.h"
#include "constants.h"

namespace mathstudio::core
{
	struct EvaluationContext;
}

double derivative(
	const Expression &expr,
	double x,
	double h = EPS_DERIV,
	mathstudio::core::EvaluationContext *ctx = nullptr);

Line tangentAt(
	const Expression &expr,
	double x0,
	mathstudio::core::EvaluationContext *ctx = nullptr);

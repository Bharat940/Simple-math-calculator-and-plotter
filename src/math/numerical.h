#pragma once

#include "expression.h"
#include "geometry.h"
#include "constants.h"

double derivative(
	const Expression &expr,
	double x,
	double h = EPS_DERIV);

Line tangentAt(const Expression &expr, double x0);

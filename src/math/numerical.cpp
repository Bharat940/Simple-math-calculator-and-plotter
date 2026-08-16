#include "numerical.h"
#include "geometry.h"
#include <cmath>
#include <limits>

#include "core/EvaluationContext.hpp"

static double evalCtx(const Expression &expr, double val, mathstudio::core::EvaluationContext *ctx)
{
	return ctx ? expr.evalWithContext(val, *ctx) : expr.eval(val);
}

double derivative(
	const Expression &expr,
	double x,
	double h,
	mathstudio::core::EvaluationContext *ctx)
{
	if (std::abs(h) < 1e-15)
	{
		return std::numeric_limits<double>::quiet_NaN();
	}

	double f_plus = evalCtx(expr, x + h, ctx);
	double f_minus = evalCtx(expr, x - h, ctx);

	if (!std::isfinite(f_plus) || !std::isfinite(f_minus))
	{
		return std::numeric_limits<double>::quiet_NaN();
	}

	return (f_plus - f_minus) / (2.0 * h);
}

Line tangentAt(const Expression &expr, double x0, mathstudio::core::EvaluationContext *ctx)
{
	double m = derivative(expr, x0, EPS_DERIV, ctx);
	double y0 = evalCtx(expr, x0, ctx);
	double b = y0 - m * x0;
	return {m, b};
}

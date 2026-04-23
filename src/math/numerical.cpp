#include "numerical.h"
#include "geometry.h"
#include <cmath>

double derivative(
	const Expression &expr,
	double x,
	double h)
{
	double f_plus = expr.eval(x + h);
	double f_minus = expr.eval(x - h);

	if (!std::isfinite(f_plus) || !std::isfinite(f_minus))
	{
		return std::numeric_limits<double>::quiet_NaN();
	}

	return (f_plus - f_minus) / (2.0 * h);
}

Line tangentAt(const Expression &expr, double x0)
{
	double m = derivative(expr, x0);
	double y0 = expr.eval(x0);
	double b = y0 - m * x0;
	return {m, b};
}

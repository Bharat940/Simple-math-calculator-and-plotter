#include "solver.h"
#include <cmath>
#include "numerical.h"

namespace
{

	template <typename Func>
	std::vector<double> solveZero(
		Func fn,
		double left,
		double right,
		double step,
		double eps)
	{
		std::vector<double> roots;

		double x1 = left;
		double y1 = fn(x1);

		for (double x2 = x1 + step; x2 <= right; x2 += step)
		{
			double y2 = fn(x2);

			if (std::isfinite(y1) && std::isfinite(y2))
			{
				double jump = std::abs(y2 - y1) / step;

				if (jump > EPS_DISCONTINUITY)
				{
					x1 = x2;
					y1 = y2;
					continue;
				}
			}

			if (std::isfinite(y1) && std::isfinite(y2) && y1 * y2 < 0)
			{
				double a = x1, b = x2;

				while (b - a > EPS_ROOT)
				{
					double mid = (a + b) / 2;
					double ym = fn(mid);

					if (y1 * ym <= 0)
					{
						b = mid;
						y2 = ym;
					}
					else
					{
						a = mid;
						y1 = ym;
					}
				}

				double root = (a + b) / 2;

				// Clamp near-zero values
				if (std::abs(root) < EPS_ROOT * 10)
				{
					root = 0.0;
				}

				if (roots.empty() || std::abs(root - roots.back()) > EPS_ROOT * 10)
				{
					roots.push_back(root);
				}
			}

			x1 = x2;
			y1 = y2;
		}

		return roots;
	}

	static SolverResult newtonRefine(
		const Expression &expr,
		double x0,
		double eps,
		int maxIter = 10)
	{
		double x = x0;

		for (int i = 0; i < maxIter; ++i)
		{
			double fx = expr.eval(x);
			double dfx = derivative(expr, x);

			if (!std::isfinite(fx) || !std::isfinite(dfx))
			{
				return {x, false, i, std::abs(fx)};
			}

			if (std::abs(dfx) < EPS_SINGULAR)
			{
				return {x, false, i, std::abs(fx)};
			}

			double xNext = x - fx / dfx;

			if (!std::isfinite(xNext))
			{
				return {x, false, i, std::abs(fx)};
			}

			if (std::abs(xNext - x) < EPS_ROOT)
			{
				double res = std::abs(expr.eval(xNext));
				return {xNext, true, i + 1, res};
			}

			x = xNext;
		}

		double res = std::abs(expr.eval(x));
		return {x, false, maxIter, res};
	}
}

std::vector<SolverResult> findRootsDetailed(
	const Expression &expr,
	double left,
	double right,
	double step,
	double eps)
{
	std::vector<SolverResult> results;

	auto roughRoots = solveZero(
		[&](double x)
		{
			return expr.eval(x);
		},
		left, right, step, eps);

	for (double r : roughRoots)
	{
		SolverResult sr = newtonRefine(expr, r, eps);

		if (std::abs(sr.x) < EPS_ROOT * 10)
		{
			sr.x = 0.0;
		}

		results.push_back(sr);
	}

	return results;
}

std::vector<double> findRoots(
	const Expression &expr,
	double left,
	double right,
	double step,
	double eps)
{
	std::vector<double> out;

	auto detailed = findRootsDetailed(expr, left, right, step, eps);
	for (const auto &sr : detailed)
	{
		out.push_back(sr.x);
	}

	return out;
}

std::vector<double> findIntersections(
	const Expression &f,
	const Expression &g,
	double left,
	double right,
	double step,
	double eps)
{
	auto roots = solveZero(
		[&](double x)
		{
			return f.eval(x) - g.eval(x);
		},
		left, right, step, eps);

	for (double &r : roots)
	{
		double x = r;

		for (int i = 0; i < 10; ++i)
		{
			double h = f.eval(x) - g.eval(x);
			double dh = derivative(f, x) - derivative(g, x);

			if (!std::isfinite(h) || !std::isfinite(dh))
			{
				break;
			}
			if (std::abs(dh) < EPS_SINGULAR)
			{
				break;
			}

			double xNext = x - h / dh;

			if (!std::isfinite(xNext))
			{
				break;
			}

			if (std::abs(xNext - x) < eps)
			{
				x = xNext;
				break;
			}

			x = xNext;
		}

		r = (std::abs(x) < eps * 10) ? 0.0 : x;
	}

	return roots;
}

std::vector<double> findExtrema(
	const Expression &expr,
	double left,
	double right,
	double step,
	double eps)
{
	return solveZero(
		[&](double x)
		{
			return derivative(expr, x);
		},
		left, right, step, eps);
}

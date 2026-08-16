#include "solver.h"
#include "numerical.h"
#include "core/EvaluationContext.hpp"

static double evalCtx(const Expression &expr, double val, mathstudio::core::EvaluationContext *ctx)
{
	return ctx ? expr.evalWithContext(val, *ctx) : expr.eval(val);
}

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
		(void)eps; // reserved for future tolerance tuning
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
		mathstudio::core::EvaluationContext *ctx,
		int maxIter = 10)
	{
		(void)eps; // tolerance set by EPS_ROOT constants
		double x = x0;

		for (int i = 0; i < maxIter; ++i)
		{
			double fx = evalCtx(expr, x, ctx);
			double dfx = derivative(expr, x, EPS_DERIV, ctx);

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
				double res = std::abs(evalCtx(expr, xNext, ctx));
				return {xNext, true, i + 1, res};
			}

			x = xNext;
		}

		double res = std::abs(evalCtx(expr, x, ctx));
		return {x, false, maxIter, res};
	}
}

std::vector<SolverResult> findRootsDetailed(
	const Expression &expr,
	double left,
	double right,
	double step,
	double eps,
	mathstudio::core::EvaluationContext *ctx)
{
	std::vector<SolverResult> results;

	auto roughRoots = solveZero(
		[&](double x)
		{
			return evalCtx(expr, x, ctx);
		},
		left, right, step, eps);

	for (double r : roughRoots)
	{
		SolverResult sr = newtonRefine(expr, r, eps, ctx);

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
	double eps,
	mathstudio::core::EvaluationContext *ctx)
{
	std::vector<double> out;

	auto detailed = findRootsDetailed(expr, left, right, step, eps, ctx);
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
	double eps,
	mathstudio::core::EvaluationContext *ctx)
{
	auto roots = solveZero(
		[&](double x)
		{
			return evalCtx(f, x, ctx) - evalCtx(g, x, ctx);
		},
		left, right, step, eps);

	for (double &r : roots)
	{
		double x = r;

		for (int i = 0; i < 10; ++i)
		{
			double h = evalCtx(f, x, ctx) - evalCtx(g, x, ctx);
			double dh = derivative(f, x, EPS_DERIV, ctx) - derivative(g, x, EPS_DERIV, ctx);

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
	double eps,
	mathstudio::core::EvaluationContext *ctx)
{
	return solveZero(
		[&](double x)
		{
			return derivative(expr, x, EPS_DERIV, ctx);
		},
		left, right, step, eps);
}

std::vector<ExtremaResult> findExtremaDetailed(
	const Expression &expr,
	double left,
	double right,
	double step,
	double eps,
	mathstudio::core::EvaluationContext *ctx)
{
	auto extremaXs = findExtrema(expr, left, right, step, eps, ctx);
	std::vector<ExtremaResult> results;

	for (double x : extremaXs)
	{
		double y = evalCtx(expr, x, ctx);
		// Approximate second derivative using central difference:
		double h = 1e-4;
		double f_plus = evalCtx(expr, x + h, ctx);
		double f_minus = evalCtx(expr, x - h, ctx);
		double fpp = (f_plus - 2.0 * y + f_minus) / (h * h);

		ExtremaKind kind = ExtremaKind::Unknown;
		if (std::isfinite(fpp))
		{
			if (fpp > 1e-4)
				kind = ExtremaKind::LocalMin;
			else if (fpp < -1e-4)
				kind = ExtremaKind::LocalMax;
			else
				kind = ExtremaKind::Saddle;
		}

		results.push_back({x, y, kind});
	}

	return results;
}

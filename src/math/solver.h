#pragma once
#include <vector>
#include "constants.h"
#include "expression.h"

struct SolverResult
{
	double x;
	bool converged;
	int iterations;
	double residual;
};

std::vector<double> findRoots(
	const Expression &expr,
	double left,
	double right,
	double step = 0.1,
	double eps = EPS_ROOT);

std::vector<SolverResult> findRootsDetailed(
	const Expression &expr,
	double left,
	double right,
	double step = 0.1,
	double eps = EPS_ROOT);

std::vector<double> findIntersections(
	const Expression &f,
	const Expression &g,
	double left,
	double right,
	double step = 0.1,
	double eps = EPS_ROOT);

std::vector<double> findExtrema(
	const Expression &expr,
	double left,
	double right,
	double step = 0.1,
	double eps = EPS_ROOT);

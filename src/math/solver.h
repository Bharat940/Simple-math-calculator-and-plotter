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

enum class ExtremaKind
{
	LocalMin,
	LocalMax,
	Saddle,
	Unknown
};

struct ExtremaResult
{
	double x;
	double y;
	ExtremaKind kind;
};

std::vector<double> findExtrema(
	const Expression &expr,
	double left,
	double right,
	double step = 0.1,
	double eps = EPS_ROOT);

std::vector<ExtremaResult> findExtremaDetailed(
	const Expression &expr,
	double left,
	double right,
	double step = 0.1,
	double eps = EPS_ROOT);

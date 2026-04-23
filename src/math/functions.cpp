#include "functions.h"
#include "constants.h"
#include <cmath>
#include <algorithm>

static std::unordered_map<std::string, FunctionDef> registry = {

    // ---------- Trigonometric ----------
    {"sin", {1, 1, [](const std::vector<double> &a)
             {
                 return EvalResult::success(std::sin(a[0]));
             }}},

    {"cos", {1, 1, [](const std::vector<double> &a)
             {
                 return EvalResult::success(std::cos(a[0]));
             }}},

    {"tan", {1, 1, [](const std::vector<double> &a)
             {
                 if (std::abs(std::cos(a[0])) < EPS_SINGULAR)
                     return EvalResult::failure("tan undefined");
                 return EvalResult::success(std::tan(a[0]));
             }}},

    {"asin", {1, 1, [](const std::vector<double> &a)
              {
                  if (a[0] < -1 || a[0] > 1)
                      return EvalResult::failure("asin domain error");
                  return EvalResult::success(std::asin(a[0]));
              }}},

    {"acos", {1, 1, [](const std::vector<double> &a)
              {
                  if (a[0] < -1 || a[0] > 1)
                      return EvalResult::failure("acos domain error");
                  return EvalResult::success(std::acos(a[0]));
              }}},

    {"atan", {1, 1, [](const std::vector<double> &a)
              {
                  return EvalResult::success(std::atan(a[0]));
              }}},

    // ---------- Hyperbolic ----------
    {"sinh", {1, 1, [](const std::vector<double> &a)
              {
                  return EvalResult::success(std::sinh(a[0]));
              }}},

    {"cosh", {1, 1, [](const std::vector<double> &a)
              {
                  return EvalResult::success(std::cosh(a[0]));
              }}},

    {"tanh", {1, 1, [](const std::vector<double> &a)
              {
                  return EvalResult::success(std::tanh(a[0]));
              }}},

    // ---------- Exponential / Logarithmic ----------
    {"exp", {1, 1, [](const std::vector<double> &a)
             {
                 return EvalResult::success(std::exp(a[0]));
             }}},

    // log(x) or log(x, base)
    {"log", {1, 2, [](const std::vector<double> &a)
             {
                 if (a.size() == 1)
                 {
                     if (a[0] <= 0)
                         return EvalResult::failure("log domain error");
                     return EvalResult::success(std::log(a[0]));
                 }
                 else
                 {
                     double x = a[0];
                     double b = a[1];

                     if (x <= 0 || b <= 0 || std::abs(b - 1.0) < EPS_EVAL)
                         return EvalResult::failure("log base domain error");

                     return EvalResult::success(std::log(x) / std::log(b));
                 }
             }}},

    {"log10", {1, 1, [](const std::vector<double> &a)
               {
                   if (a[0] <= 0)
                       return EvalResult::failure("log10 domain error");
                   return EvalResult::success(std::log10(a[0]));
               }}},

    // ---------- Algebraic ----------
    {"sqrt", {1, 1, [](const std::vector<double> &a)
              {
                  if (a[0] < 0)
                      return EvalResult::failure("sqrt domain error");
                  return EvalResult::success(std::sqrt(a[0]));
              }}},

    {"abs", {1, 1, [](const std::vector<double> &a)
             {
                 return EvalResult::success(std::abs(a[0]));
             }}},

    {"floor", {1, 1, [](const std::vector<double> &a)
               {
                   return EvalResult::success(std::floor(a[0]));
               }}},

    {"ceil", {1, 1, [](const std::vector<double> &a)
              {
                  return EvalResult::success(std::ceil(a[0]));
              }}},

    // ---------- Binary ----------
    {"pow", {2, 2, [](const std::vector<double> &a)
             {
                 return EvalResult::success(std::pow(a[0], a[1]));
             }}},

    {"max", {2, 2, [](const std::vector<double> &a)
             {
                 return EvalResult::success(std::max(a[0], a[1]));
             }}},

    {"min", {2, 2, [](const std::vector<double> &a)
             {
                 return EvalResult::success(std::min(a[0], a[1]));
             }}}};

const std::unordered_map<std::string, FunctionDef> &getFunctionRegistry()
{
    return registry;
}

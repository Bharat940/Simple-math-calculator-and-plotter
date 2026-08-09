#include "FunctionRegistry.hpp"
#include <cmath>
#include <algorithm>
#include <stdexcept>

namespace mathstudio::core
{

    FunctionRegistry::FunctionRegistry()
    {
        registerFunction({"sin", 1, 1, "Trig", "Sine function", [](const std::vector<MathValue> &args)
                          {
                              if (args[0].isComplex())
                                  return MathValue::complex(std::sin(args[0].getComplex()));
                              return MathValue::real(std::sin(args[0].getReal()));
                          }});
        registerFunction({"cos", 1, 1, "Trig", "Cosine function", [](const std::vector<MathValue> &args)
                          {
                              if (args[0].isComplex())
                                  return MathValue::complex(std::cos(args[0].getComplex()));
                              return MathValue::real(std::cos(args[0].getReal()));
                          }});
        registerFunction({"tan", 1, 1, "Trig", "Tangent function", [](const std::vector<MathValue> &args)
                          {
                              if (args[0].isComplex())
                                  return MathValue::complex(std::tan(args[0].getComplex()));
                              return MathValue::real(std::tan(args[0].getReal()));
                          }});
        registerFunction({"asin", 1, 1, "Trig", "Inverse sine function", [](const std::vector<MathValue> &args)
                          {
                              double v = args[0].getReal();
                              if (v < -1.0 || v > 1.0)
                                  return MathValue::undefined();
                              return MathValue::real(std::asin(v));
                          }});
        registerFunction({"acos", 1, 1, "Trig", "Inverse cosine function", [](const std::vector<MathValue> &args)
                          {
                              double v = args[0].getReal();
                              if (v < -1.0 || v > 1.0)
                                  return MathValue::undefined();
                              return MathValue::real(std::acos(v));
                          }});
        registerFunction({"atan", 1, 1, "Trig", "Inverse tangent function", [](const std::vector<MathValue> &args)
                          {
                              return MathValue::real(std::atan(args[0].getReal()));
                          }});
        registerFunction({"sinh", 1, 1, "Hyperbolic", "Hyperbolic sine", [](const std::vector<MathValue> &args)
                          {
                              return MathValue::real(std::sinh(args[0].getReal()));
                          }});
        registerFunction({"cosh", 1, 1, "Hyperbolic", "Hyperbolic cosine", [](const std::vector<MathValue> &args)
                          {
                              return MathValue::real(std::cosh(args[0].getReal()));
                          }});
        registerFunction({"tanh", 1, 1, "Hyperbolic", "Hyperbolic tangent", [](const std::vector<MathValue> &args)
                          {
                              return MathValue::real(std::tanh(args[0].getReal()));
                          }});
        registerFunction({"exp", 1, 1, "Exponential", "Exponential function e^x", [](const std::vector<MathValue> &args)
                          {
                              if (args[0].isComplex())
                                  return MathValue::complex(std::exp(args[0].getComplex()));
                              return MathValue::real(std::exp(args[0].getReal()));
                          }});
        registerFunction({"log", 1, 2, "Exponential", "Natural logarithm / Log base b", [](const std::vector<MathValue> &args)
                          {
                              if (args.size() == 1)
                              {
                                  double v = args[0].getReal();
                                  if (v <= 0.0)
                                      return MathValue::undefined();
                                  return MathValue::real(std::log(v));
                              }
                              double x = args[0].getReal();
                              double base = args[1].getReal();
                              if (x <= 0.0 || base <= 0.0 || base == 1.0)
                                  return MathValue::undefined();
                              return MathValue::real(std::log(x) / std::log(base));
                          }});
        registerFunction({"log10", 1, 1, "Exponential", "Base 10 logarithm", [](const std::vector<MathValue> &args)
                          {
                              double v = args[0].getReal();
                              if (v <= 0.0)
                                  return MathValue::undefined();
                              return MathValue::real(std::log10(v));
                          }});
        registerFunction({"sqrt", 1, 1, "Algebraic", "Square root", [](const std::vector<MathValue> &args)
                          {
                              double v = args[0].getReal();
                              if (v < 0.0)
                                  return MathValue::undefined();
                              return MathValue::real(std::sqrt(v));
                          }});
        registerFunction({"abs", 1, 1, "Algebraic", "Absolute value", [](const std::vector<MathValue> &args)
                          {
                              if (args[0].isComplex())
                                  return MathValue::real(std::abs(args[0].getComplex()));
                              return MathValue::real(std::abs(args[0].getReal()));
                          }});
        registerFunction({"floor", 1, 1, "Algebraic", "Floor function", [](const std::vector<MathValue> &args)
                          {
                              return MathValue::real(std::floor(args[0].getReal()));
                          }});
        registerFunction({"ceil", 1, 1, "Algebraic", "Ceiling function", [](const std::vector<MathValue> &args)
                          {
                              return MathValue::real(std::ceil(args[0].getReal()));
                          }});
        registerFunction({"pow", 2, 2, "Algebraic", "Power x^y", [](const std::vector<MathValue> &args)
                          {
                              return MathValue::real(std::pow(args[0].getReal(), args[1].getReal()));
                          }});
        registerFunction({"max", 2, 2, "Algebraic", "Maximum of two values", [](const std::vector<MathValue> &args)
                          {
                              return MathValue::real(std::max(args[0].getReal(), args[1].getReal()));
                          }});
        registerFunction({"min", 2, 2, "Algebraic", "Minimum of two values", [](const std::vector<MathValue> &args)
                          {
                              return MathValue::real(std::min(args[0].getReal(), args[1].getReal()));
                          }});
    }

    FunctionRegistry &FunctionRegistry::instance()
    {
        static FunctionRegistry instance;
        return instance;
    }

    void FunctionRegistry::registerFunction(const CoreFunctionDef &def)
    {
        functions[def.name] = def;
    }

    bool FunctionRegistry::has(const std::string &name) const
    {
        return functions.find(name) != functions.end();
    }

    const CoreFunctionDef &FunctionRegistry::get(const std::string &name) const
    {
        auto it = functions.find(name);
        if (it != functions.end())
        {
            return it->second;
        }
        throw std::runtime_error("Unknown function: " + name);
    }

    MathValue FunctionRegistry::call(const std::string &name, const std::vector<MathValue> &args) const
    {
        const auto &def = get(name);
        int numArgs = static_cast<int>(args.size());
        if (numArgs < def.minArity || numArgs > def.maxArity)
        {
            return MathValue::undefined();
        }
        return def.evalFn(args);
    }

    const std::unordered_map<std::string, CoreFunctionDef> &FunctionRegistry::getAll() const
    {
        return functions;
    }

} // namespace mathstudio::core

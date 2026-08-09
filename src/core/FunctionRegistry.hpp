#pragma once

#include "MathValue.hpp"
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>

namespace mathstudio::core
{

    struct CoreFunctionDef
    {
        std::string name;
        int minArity = 1;
        int maxArity = 1;
        std::string category; // Trig, Hyperbolic, Exponential, Algebraic, Complex, Signal, Statistics
        std::string description;
        std::function<MathValue(const std::vector<MathValue> &)> evalFn;
    };

    class FunctionRegistry
    {
    private:
        std::unordered_map<std::string, CoreFunctionDef> functions;

    public:
        FunctionRegistry();
        static FunctionRegistry &instance();
        void registerFunction(const CoreFunctionDef &def);
        bool has(const std::string &name) const;
        const CoreFunctionDef &get(const std::string &name) const;
        MathValue call(const std::string &name, const std::vector<MathValue> &args) const;
        const std::unordered_map<std::string, CoreFunctionDef> &getAll() const;
    };

} // namespace mathstudio::core

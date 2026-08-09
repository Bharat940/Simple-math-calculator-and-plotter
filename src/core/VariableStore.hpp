#pragma once

#include "MathValue.hpp"
#include <unordered_map>
#include <string>

class VariableStore
{
private:
    std::unordered_map<std::string, MathValue> vars;

    // Fast direct variable slots
    MathValue xVal = MathValue::real(0.0);
    MathValue tVal = MathValue::real(0.0);
    MathValue nVal = MathValue::real(0.0);
    MathValue thetaVal = MathValue::real(0.0);
    MathValue zVal = MathValue::real(0.0);
    MathValue ansVal = MathValue::real(0.0);

public:
    VariableStore();
    void set(const std::string &name, const MathValue &val);
    MathValue get(const std::string &name) const;
    bool has(const std::string &name) const;
    void remove(const std::string &name);
    void clear();
    const std::unordered_map<std::string, MathValue> &getAll() const;

    // Fast O(1) direct accessors
    inline MathValue getX() const { return xVal; }
    inline MathValue getT() const { return tVal; }
    inline MathValue getN() const { return nVal; }
};

#include "VariableStore.hpp"

VariableStore::VariableStore()
{
    vars["x"] = xVal;
    vars["t"] = tVal;
    vars["n"] = nVal;
    vars["theta"] = thetaVal;
    vars["z"] = zVal;
    vars["ans"] = ansVal;
}

void VariableStore::set(const std::string &name, const MathValue &val)
{
    vars[name] = val;
    if (name.length() == 1)
    {
        char c = name[0];
        if (c == 'x')
        {
            xVal = val;
            return;
        }
        if (c == 't')
        {
            tVal = val;
            return;
        }
        if (c == 'n')
        {
            nVal = val;
            return;
        }
        if (c == 'z')
        {
            zVal = val;
            return;
        }
    }
    else if (name == "theta")
    {
        thetaVal = val;
    }
    else if (name == "ans")
    {
        ansVal = val;
    }
}

MathValue VariableStore::get(const std::string &name) const
{
    if (name.length() == 1)
    {
        char c = name[0];
        if (c == 'x')
            return xVal;
        if (c == 't')
            return tVal;
        if (c == 'n')
            return nVal;
        if (c == 'z')
            return zVal;
    }
    else if (name == "theta")
    {
        return thetaVal;
    }
    else if (name == "ans")
    {
        return ansVal;
    }

    auto it = vars.find(name);
    if (it != vars.end())
    {
        return it->second;
    }
    return MathValue::undefined();
}

bool VariableStore::has(const std::string &name) const
{
    if (name.length() == 1)
    {
        char c = name[0];
        if (c == 'x' || c == 't' || c == 'n' || c == 'z')
            return true;
    }
    else if (name == "theta" || name == "ans")
    {
        return true;
    }
    return vars.find(name) != vars.end();
}

void VariableStore::remove(const std::string &name)
{
    vars.erase(name);
}

void VariableStore::clear()
{
    vars.clear();
    xVal = MathValue::real(0.0);
    tVal = MathValue::real(0.0);
    nVal = MathValue::real(0.0);
    thetaVal = MathValue::real(0.0);
    zVal = MathValue::real(0.0);
    ansVal = MathValue::real(0.0);
    vars["x"] = xVal;
    vars["t"] = tVal;
    vars["n"] = nVal;
    vars["theta"] = thetaVal;
    vars["z"] = zVal;
    vars["ans"] = ansVal;
}

const std::unordered_map<std::string, MathValue> &VariableStore::getAll() const
{
    return vars;
}

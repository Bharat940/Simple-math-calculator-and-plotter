#include "MathValue.hpp"
#include <sstream>
#include <iomanip>
#include <cmath>

MathValue MathValue::real(double v)
{
    MathValue val;
    val.value = v;
    return val;
}

MathValue MathValue::complex(double re, double im)
{
    MathValue val;
    val.value = std::complex<double>(re, im);
    return val;
}

MathValue MathValue::complex(std::complex<double> c)
{
    MathValue val;
    val.value = c;
    return val;
}

MathValue MathValue::boolean(bool b)
{
    MathValue val;
    val.value = b;
    return val;
}

MathValue MathValue::mat(Matrix m)
{
    MathValue val;
    val.value = std::move(m);
    return val;
}

MathValue MathValue::vectorVal(Vector v)
{
    MathValue val;
    val.value = std::move(v);
    return val;
}

MathValue MathValue::inf(int sign)
{
    MathValue val;
    val.isInf = true;
    val.infSign = (sign >= 0) ? 1 : -1;
    return val;
}

MathValue MathValue::undefined()
{
    MathValue val;
    val.isUndef = true;
    return val;
}

double MathValue::getReal() const
{
    if (isUndef)
        return std::numeric_limits<double>::quiet_NaN();
    if (isInf)
        return infSign * std::numeric_limits<double>::infinity();
    if (std::holds_alternative<double>(value))
    {
        return std::get<double>(value);
    }
    if (std::holds_alternative<std::complex<double>>(value))
    {
        return std::get<std::complex<double>>(value).real();
    }
    if (std::holds_alternative<bool>(value))
    {
        return std::get<bool>(value) ? 1.0 : 0.0;
    }
    return 0.0;
}

std::complex<double> MathValue::getComplex() const
{
    if (std::holds_alternative<std::complex<double>>(value))
    {
        return std::get<std::complex<double>>(value);
    }
    return std::complex<double>(getReal(), 0.0);
}

bool MathValue::isComplex() const
{
    return std::holds_alternative<std::complex<double>>(value);
}

bool MathValue::isReal() const
{
    return std::holds_alternative<double>(value);
}

bool MathValue::isNumber() const
{
    return !isUndef && !isInf && (isReal() || isComplex());
}

bool MathValue::isBool() const
{
    return std::holds_alternative<bool>(value);
}

bool MathValue::isMatrix() const
{
    return std::holds_alternative<Matrix>(value);
}

bool MathValue::isVector() const
{
    return std::holds_alternative<Vector>(value);
}

std::string MathValue::toString() const
{
    if (isUndef)
        return "undefined";
    if (isInf)
        return (infSign < 0) ? "-infinity" : "infinity";

    if (std::holds_alternative<double>(value))
    {
        std::ostringstream ss;
        ss << std::get<double>(value);
        return ss.str();
    }
    if (std::holds_alternative<std::complex<double>>(value))
    {
        auto c = std::get<std::complex<double>>(value);
        std::ostringstream ss;
        ss << c.real();
        if (c.imag() >= 0)
            ss << " + " << c.imag() << "i";
        else
            ss << " - " << (-c.imag()) << "i";
        return ss.str();
    }
    if (std::holds_alternative<bool>(value))
    {
        return std::get<bool>(value) ? "true" : "false";
    }
    if (std::holds_alternative<Vector>(value))
    {
        const auto &vec = std::get<Vector>(value);
        std::string s = "[";
        for (size_t i = 0; i < vec.size(); ++i)
        {
            s += std::to_string(vec[i]);
            if (i + 1 < vec.size())
                s += ", ";
        }
        s += "]";
        return s;
    }
    if (std::holds_alternative<Matrix>(value))
    {
        return "[Matrix]";
    }
    return "undefined";
}

MathValue MathValue::operator+(const MathValue &rhs) const
{
    if (isUndef || rhs.isUndef)
        return undefined();
    if (isInf || rhs.isInf)
        return inf();
    if (isComplex() || rhs.isComplex())
    {
        return complex(getComplex() + rhs.getComplex());
    }
    return real(getReal() + rhs.getReal());
}

MathValue MathValue::operator-(const MathValue &rhs) const
{
    if (isUndef || rhs.isUndef)
        return undefined();
    if (isInf || rhs.isInf)
        return inf();
    if (isComplex() || rhs.isComplex())
    {
        return complex(getComplex() - rhs.getComplex());
    }
    return real(getReal() - rhs.getReal());
}

MathValue MathValue::operator*(const MathValue &rhs) const
{
    if (isUndef || rhs.isUndef)
        return undefined();
    if (isInf || rhs.isInf)
        return inf();
    if (isComplex() || rhs.isComplex())
    {
        return complex(getComplex() * rhs.getComplex());
    }
    return real(getReal() * rhs.getReal());
}

MathValue MathValue::operator/(const MathValue &rhs) const
{
    if (isUndef || rhs.isUndef)
        return undefined();
    if (rhs.getReal() == 0.0 && !rhs.isComplex())
        return undefined();
    if (isComplex() || rhs.isComplex())
    {
        return complex(getComplex() / rhs.getComplex());
    }
    return real(getReal() / rhs.getReal());
}

MathValue MathValue::operator-() const
{
    if (isUndef)
        return undefined();
    if (isInf)
        return inf(-infSign);
    if (isComplex())
        return complex(-getComplex());
    return real(-getReal());
}

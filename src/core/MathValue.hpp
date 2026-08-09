#pragma once

#include <complex>
#include <vector>
#include <variant>
#include <string>
#include <stdexcept>
#include <limits>

using Matrix = std::vector<std::vector<double>>;
using Vector = std::vector<double>;

using MathValueVariant = std::variant<
    double,
    std::complex<double>,
    Matrix,
    Vector,
    bool>;

struct MathValue
{
    MathValueVariant value = 0.0;
    bool isInf = false;
    int infSign = 1;
    bool isUndef = false;

    MathValue() = default;

    static MathValue real(double v);
    static MathValue complex(double re, double im);
    static MathValue complex(std::complex<double> c);
    static MathValue boolean(bool b);
    static MathValue mat(Matrix m);
    static MathValue vectorVal(Vector v);
    static MathValue inf(int sign = 1);
    static MathValue undefined();

    double getReal() const;
    std::complex<double> getComplex() const;
    bool isComplex() const;
    bool isNumber() const;
    bool isReal() const;
    bool isBool() const;
    bool isMatrix() const;
    bool isVector() const;
    std::string toString() const;

    MathValue operator+(const MathValue &rhs) const;
    MathValue operator-(const MathValue &rhs) const;
    MathValue operator*(const MathValue &rhs) const;
    MathValue operator/(const MathValue &rhs) const;
    MathValue operator-() const;
};

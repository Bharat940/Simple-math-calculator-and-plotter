#pragma once

#include <string>
#include <vector>

struct DiagnosticError
{
    std::string message;
    size_t line = 1;
    size_t column = 0;
    std::string hint;
};

class DiagnosticsReporter
{
private:
    std::vector<DiagnosticError> errors;

public:
    DiagnosticsReporter() = default;
    void reportError(const std::string &msg, size_t line, size_t col, const std::string &hint = "");
    bool hasErrors() const { return !errors.empty(); }
    const std::vector<DiagnosticError> &getErrors() const { return errors; }
    std::string formatFormatted(const std::string &sourceExpr) const;
    void clear() { errors.clear(); }
};

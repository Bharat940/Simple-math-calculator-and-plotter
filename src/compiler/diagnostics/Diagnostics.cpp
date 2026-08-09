#include "Diagnostics.hpp"
#include <sstream>

void DiagnosticsReporter::reportError(const std::string &msg, size_t line, size_t col, const std::string &hint)
{
    errors.push_back({msg, line, col, hint});
}

std::string DiagnosticsReporter::formatFormatted(const std::string &sourceExpr) const
{
    if (errors.empty())
        return "";
    std::ostringstream ss;
    for (const auto &err : errors)
    {
        ss << sourceExpr << "\n";
        if (err.column > 0)
        {
            for (size_t i = 0; i < err.column - 1; ++i)
                ss << " ";
        }
        ss << "^ Error: " << err.message;
        if (!err.hint.empty())
        {
            ss << " (" << err.hint << ")";
        }
        ss << "\n";
    }
    return ss.str();
}

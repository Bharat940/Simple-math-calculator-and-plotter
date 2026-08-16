#include "Diagnostics.hpp"
#include <sstream>

// ─── Errors ───────────────────────────────────────────────────────────────────
void DiagnosticsReporter::reportError(const std::string &msg, size_t line,
                                      size_t col, const std::string &hint)
{
    errors_.push_back({msg, line, col, hint});
}

// ─── Warnings ─────────────────────────────────────────────────────────────────
void DiagnosticsReporter::reportWarning(const std::string &msg,
                                        const std::string &source,
                                        const std::string &hint)
{
    // Deduplicate: don't repeat the same warning for the same source more than once
    for (const auto &w : warnings_)
    {
        if (w.message == msg && w.source == source)
            return;
    }
    warnings_.push_back({msg, source, hint});
}

// ─── Timing ───────────────────────────────────────────────────────────────────
void DiagnosticsReporter::recordTiming(const std::string &label, double ms)
{
    // Exponential moving average: weight new sample lightly so display is stable
    constexpr double alpha = 0.05;
    for (auto &t : timings_)
    {
        if (t.label == label)
        {
            t.ms = t.ms * (1.0 - alpha) + ms * alpha;
            return;
        }
    }
    // First time we see this label — add it directly
    timings_.push_back({label, ms});
}

// ─── Legacy ───────────────────────────────────────────────────────────────────
std::string DiagnosticsReporter::formatFormatted(const std::string &sourceExpr) const
{
    if (errors_.empty())
        return "";
    std::ostringstream ss;
    for (const auto &err : errors_)
    {
        ss << sourceExpr << "\n";
        if (err.column > 0)
        {
            for (size_t i = 0; i < err.column - 1; ++i)
                ss << " ";
        }
        ss << "^ Error: " << err.message;
        if (!err.hint.empty())
            ss << " (" << err.hint << ")";
        ss << "\n";
    }
    return ss.str();
}

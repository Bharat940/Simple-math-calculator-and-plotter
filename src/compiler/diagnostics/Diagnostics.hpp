#pragma once

#include <string>
#include <vector>

// ─── Error ────────────────────────────────────────────────────────────────────
struct DiagnosticError
{
    std::string message;
    size_t line = 1;
    size_t column = 0;
    std::string hint;
};

// ─── Warning ──────────────────────────────────────────────────────────────────
struct DiagnosticWarning
{
    std::string message;
    std::string source; // expression that triggered the warning
    std::string hint;
};

// ─── Timing ───────────────────────────────────────────────────────────────────
struct TimingEntry
{
    std::string label; // e.g. "Parse: sin(x)"
    double ms = 0.0;   // duration in milliseconds
};

// ─── Reporter ─────────────────────────────────────────────────────────────────
class DiagnosticsReporter
{
private:
    std::vector<DiagnosticError> errors_;
    std::vector<DiagnosticWarning> warnings_;
    std::vector<TimingEntry> timings_;

public:
    DiagnosticsReporter() = default;

    // Errors
    void reportError(const std::string &msg, size_t line, size_t col,
                     const std::string &hint = "");
    bool hasErrors() const { return !errors_.empty(); }
    const std::vector<DiagnosticError> &getErrors() const { return errors_; }
    void clearErrors() { errors_.clear(); }

    // Warnings
    void reportWarning(const std::string &msg, const std::string &source = "",
                       const std::string &hint = "");
    bool hasWarnings() const { return !warnings_.empty(); }
    const std::vector<DiagnosticWarning> &getWarnings() const { return warnings_; }
    void clearWarnings() { warnings_.clear(); }

    // Timing
    void recordTiming(const std::string &label, double ms);
    const std::vector<TimingEntry> &getTimings() const { return timings_; }
    void clearTimings() { timings_.clear(); }

    // Convenience: clear everything
    void clear()
    {
        errors_.clear();
        warnings_.clear();
        timings_.clear();
    }

    // Legacy helpers (kept for existing call-sites)
    std::string formatFormatted(const std::string &sourceExpr) const;
};

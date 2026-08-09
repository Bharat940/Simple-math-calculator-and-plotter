#pragma once

#include "VariableStore.hpp"
#include "FunctionRegistry.hpp"
#include "../compiler/diagnostics/Diagnostics.hpp"

struct EvaluationContext
{
    const VariableStore &vars;
    const mathstudio::core::FunctionRegistry &functions;
    DiagnosticsReporter &diagnostics;

    EvaluationContext(const VariableStore &vStore, const mathstudio::core::FunctionRegistry &fReg, DiagnosticsReporter &diag)
        : vars(vStore), functions(fReg), diagnostics(diag) {}
};

#pragma once

#include "VariableStore.hpp"
#include "FunctionRegistry.hpp"
#include "../compiler/diagnostics/Diagnostics.hpp"

namespace mathstudio::core
{

    struct EvaluationContext
    {
        VariableStore &vars;
        const FunctionRegistry &functions;
        DiagnosticsReporter &diagnostics;

        EvaluationContext(VariableStore &vStore, const FunctionRegistry &fReg, DiagnosticsReporter &diag)
            : vars(vStore), functions(fReg), diagnostics(diag) {}
    };

} // namespace mathstudio::core

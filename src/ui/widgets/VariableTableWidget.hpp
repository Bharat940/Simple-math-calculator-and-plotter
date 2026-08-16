#pragma once

#include "ui/widgets/IWidget.hpp"
#include <map>
#include <string>
#include <unordered_set>

namespace mathstudio::ui
{

    struct ParamConfig
    {
        double value = 1.0;
        float minVal = -10.0f;
        float maxVal = 10.0f;
        float step = 0.05f;
        bool animating = false;
        float animSpeed = 2.0f; // units per second
        bool animForward = true;
    };

    class VariableTableWidget : public IWidget
    {
    public:
        VariableTableWidget();

        void render(core::EvaluationContext &context) override;

        void addParameter(const std::string &name, double defaultVal = 1.0);
        bool hasParameter(const std::string &name) const;

    private:
        char m_searchBuffer[128] = {0};
        std::map<std::string, ParamConfig> m_userParams;
        std::unordered_set<std::string> m_manualParams;
    };

} // namespace mathstudio::ui

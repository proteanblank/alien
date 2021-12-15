#pragma once

#include "EngineInterface/SelectionShallowData.h"
#include "EngineImpl/Definitions.h"
#include "Definitions.h"

class _ManipulatorWindow
{
public:
    _ManipulatorWindow(
        EditorModel const& editorModel,
        SimulationController const& simController);
    ~_ManipulatorWindow();

    void process();

    bool isOn() const;
    void setOn(bool value);

private:
    bool colorButton(std::string id, uint32_t cellColor);
    bool hasSelectionChanged(SelectionShallowData const& selection) const;

    EditorModel _editorModel;
    SimulationController _simController;

    bool _on = false;
    bool _includeClusters = true;
    float _angle = 0;
    float _angularVel = 0;
    boost::optional<SelectionShallowData> _lastSelection;
};
#pragma once

#include "EngineInterface/Definitions.h"
#include "Definitions.h"

class _OpenPatternDialog
{
public:
    _OpenPatternDialog(
        EditorModel const& editorModel,
        SimulationController const& simController,
        Viewport const& viewport);

    void process();

    void show();

private:
    EditorModel _editorModel;
    SimulationController _simController;
    Viewport _viewport;
};

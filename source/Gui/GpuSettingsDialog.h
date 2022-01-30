#pragma once

#include "EngineInterface/Definitions.h"
#include "EngineInterface/GpuSettings.h"
#include "Definitions.h"

class _GpuSettingsDialog
{
public:
    _GpuSettingsDialog(SimulationController const& simController);

    ~_GpuSettingsDialog();

    void process();

    void show();

private:
    SimulationController _simController;

    bool _show = false;
    GpuSettings _gpuSettings;
};
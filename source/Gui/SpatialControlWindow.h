#pragma once

#include "EngineInterface/Definitions.h"
#include "EngineInterface/Descriptions.h"

#include "Definitions.h"
#include "AlienWindow.h"

class _SpatialControlWindow : public _AlienWindow
{
public:
    _SpatialControlWindow(SimulationController const& simController, Viewport const& viewport);

private:
    void processIntern();
    void processZoomInButton();
    void processZoomOutButton();
    void processResizeButton();
    void processZoomSensitivitySlider();

    void processResizeDialog();

    void onResizing();

    SimulationController _simController;
    Viewport _viewport;

    bool _showResizeDialog = false;
    bool _scaleContent = false;
    int _width = 0;
    int _height = 0;
};
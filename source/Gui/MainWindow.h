#pragma once

#include "EngineImpl/Definitions.h"
#include "Definitions.h"

struct GLFWvidmode;
class _MainWindow
{
public:
    GLFWwindow* init(SimulationController const& simController);
    void mainLoop(GLFWwindow* window);
    void shutdown(GLFWwindow* window);

private:
    struct GlfwData
    {
        GLFWwindow* window;
        GLFWvidmode const* mode;
        char const* glsl_version;
    };
    GlfwData initGlfw();

    void processMenubar();
    void processToolbar();
    void processDialogs();
    void processWindows();

    void onOpenSimulation();
    void onSaveSimulation();
    void onRunSimulation();
    void onPauseSimulation();

    void onSimulationParameters();

    SimulationController _simController;
    SimulationView _simulationView;
    TemporalControlWindow _temporalControlWindow;
    StyleRepository _styleRepository;
    bool _onClose = false;
};
#pragma once

#include "Base/Definitions.h"

class _MainWindow;
using MainWindow = std::shared_ptr<_MainWindow>;

class _AlienWindow;
using AlienWindow = std::shared_ptr<_AlienWindow>;

class _SimulationView;
using SimulationView= std::shared_ptr<_SimulationView>;

class _Shader;
using Shader = std::shared_ptr<_Shader>;

class _SimulationScrollbar;
using SimulationScrollbar = std::shared_ptr<_SimulationScrollbar>;

class _Viewport;
using Viewport = std::shared_ptr<_Viewport>;

class StyleRepository;

class _TemporalControlWindow;
using TemporalControlWindow = std::shared_ptr<_TemporalControlWindow>;

class _SpatialControlWindow;
using SpatialControlWindow = std::shared_ptr<_SpatialControlWindow>;

class _SimulationParametersWindow;
using SimulationParametersWindow = std::shared_ptr<_SimulationParametersWindow>;

class _StatisticsWindow;
using StatisticsWindow = std::shared_ptr<_StatisticsWindow>;

class _ModeController;
using ModeController = std::shared_ptr<_ModeController>;

class _GpuSettingsDialog;
using GpuSettingsDialog = std::shared_ptr<_GpuSettingsDialog>;

class _NewSimulationDialog;
using NewSimulationDialog = std::shared_ptr<_NewSimulationDialog>;

class _StartupWindow;
using StartupWindow = std::shared_ptr<_StartupWindow>;

class _FlowGeneratorWindow;
using FlowGeneratorWindow = std::shared_ptr<_FlowGeneratorWindow>;

class _AboutDialog;
using AboutDialog = std::shared_ptr<_AboutDialog>;

class _ColorizeDialog;
using ColorizeDialog = std::shared_ptr<_ColorizeDialog>;

class _LogWindow;
using LogWindow = std::shared_ptr<_LogWindow>;

class _SimpleLogger;
using SimpleLogger = std::shared_ptr<_SimpleLogger>;

class _FileLogger;
using FileLogger = std::shared_ptr<_FileLogger>;

class _UiController;
using UiController = std::shared_ptr<_UiController>;

class _AutosaveController;
using AutosaveController = std::shared_ptr<_AutosaveController>;

class _GettingStartedWindow;
using GettingStartedWindow = std::shared_ptr<_GettingStartedWindow>;

class _OpenSimulationDialog;
using OpenSimulationDialog = std::shared_ptr<_OpenSimulationDialog>;

class _SaveSimulationDialog;
using SaveSimulationDialog = std::shared_ptr<_SaveSimulationDialog>;

class _DisplaySettingsDialog;
using DisplaySettingsDialog = std::shared_ptr<_DisplaySettingsDialog>;

class _EditorModel;
using EditorModel = std::shared_ptr<_EditorModel>;

class _EditorController;
using EditorController = std::shared_ptr<_EditorController>;

class _SelectionWindow;
using SelectionWindow = std::shared_ptr<_SelectionWindow>;

class _ManipulatorWindow;
using ManipulatorWindow = std::shared_ptr<_ManipulatorWindow>;

class _WindowController;
using WindowController = std::shared_ptr<_WindowController>;

class _SavePatternDialog;
using SavePatternDialog = std::shared_ptr<_SavePatternDialog>;

class _SaveSymbolsDialog;
using SaveSymbolsDialog = std::shared_ptr<_SaveSymbolsDialog>;

class _OpenPatternDialog;
using OpenPatternDialog = std::shared_ptr<_OpenPatternDialog>;

class _OpenSymbolsDialog;
using OpenSymbolsDialog = std::shared_ptr<_OpenSymbolsDialog>;

class _InspectorWindow;
using InspectorWindow = std::shared_ptr<_InspectorWindow>;

class _CreatorWindow;
using CreatorWindow = std::shared_ptr<_CreatorWindow>;

class _MultiplierWindow;
using MultiplierWindow = std::shared_ptr<_MultiplierWindow>;

class _SymbolsWindow;
using SymbolsWindow = std::shared_ptr<_SymbolsWindow>;

struct GLFWvidmode;
struct GLFWwindow;
struct ImFont;

struct TextureData
{
    unsigned int textureId;
    int width;
    int height;
};

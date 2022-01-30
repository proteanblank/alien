#pragma once

#include <cstdint>
#include <atomic>
#include <vector>

#if defined(_WIN32)
#define NOMINMAX
#include <windows.h>
#endif
#include <GL/gl.h>

#include "EngineInterface/OverallStatistics.h"
#include "EngineInterface/Settings.h"
#include "EngineInterface/GpuSettings.h"
#include "EngineInterface/FlowFieldSettings.h"
#include "EngineInterface/SelectionShallowData.h"
#include "EngineInterface/ShallowUpdateSelectionData.h"

#include "Definitions.cuh"

class _CudaSimulationAdapter
{
public:
    static void initCuda();

    _CudaSimulationAdapter(uint64_t timestep, Settings const& settings, GpuSettings const& gpuSettings);
    ~_CudaSimulationAdapter();

    void* registerImageResource(GLuint image);

    void calcTimestep();

    void drawVectorGraphics(
        float2 const& rectUpperLeft,
        float2 const& rectLowerRight,
        void* cudaResource,
        int2 const& imageSize,
        double zoom);
    void
    getSimulationData(int2 const& rectUpperLeft, int2 const& rectLowerRight, DataAccessTO const& dataTO);
    void getSelectedSimulationData(bool includeClusters, DataAccessTO const& dataTO);
    void getInspectedSimulationData(
        std::vector<uint64_t> entityIds,
        DataAccessTO const& dataTO);
    void
    getOverlayData(int2 const& rectUpperLeft, int2 const& rectLowerRight, DataAccessTO const& dataTO);
    void addAndSelectSimulationData(DataAccessTO const& dataTO);
    void setSimulationData(DataAccessTO const& dataTO);
    void removeSelectedEntities(bool includeClusters);
    void changeInspectedSimulationData(DataAccessTO const& changeDataTO);

    void applyForce(ApplyForceData const& applyData);
    void switchSelection(PointSelectionData const& switchData);
    void swapSelection(PointSelectionData const& selectionData);
    void setSelection(AreaSelectionData const& selectionData);
    SelectionShallowData getSelectionShallowData();
    void shallowUpdateSelectedEntities(ShallowUpdateSelectionData const& shallowUpdateData);
    void removeSelection();
    void updateSelection();
    void colorSelectedEntities(unsigned char color, bool includeClusters);
    void reconnectSelectedEntities();

    void setGpuConstants(GpuSettings const& cudaConstants);
    void setSimulationParameters(SimulationParameters const& parameters);
    void setSimulationParametersSpots(SimulationParametersSpots const& spots);
    void setFlowFieldSettings(FlowFieldSettings const& settings);

    ArraySizes getArraySizes() const;

    OverallStatistics getMonitorData();
    uint64_t getCurrentTimestep() const;
    void setCurrentTimestep(uint64_t timestep);

    void clear();

    void resizeArraysIfNecessary(ArraySizes const& additionals);

private:
    void syncAndCheck();
    void copyDataTOtoDevice(DataAccessTO const& dataTO);
    void copyDataTOtoHost(DataAccessTO const& dataTO);
    void automaticResizeArrays();
    void resizeArrays(ArraySizes const& additionals);

    std::atomic<uint64_t> _currentTimestep;
    GpuSettings _gpuSettings;
    FlowFieldSettings _flowFieldSettings;

    std::shared_ptr<SimulationData> _cudaSimulationData;
    std::shared_ptr<RenderingData> _cudaRenderingData;
    std::shared_ptr<SimulationResult> _cudaSimulationResult;
    std::shared_ptr<SelectionResult> _cudaSelectionResult;
    std::shared_ptr<DataAccessTO> _cudaAccessTO;
    std::shared_ptr<CudaMonitorData> _cudaMonitorData;

    SimulationKernelsLauncher _simulationKernels;
    DataAccessKernelsLauncher _dataAccessKernels;
    GarbageCollectorKernelsLauncher _garbageCollectorKernels;
    RenderingKernelsLauncher _renderingKernels;
    EditKernelsLauncher _editKernels;
    MonitorKernelsLauncher _monitorKernels;
};

﻿#include "DataAccessKernelsLauncher.cuh"

#include "DataAccessKernels.cuh"
#include "GarbageCollectorKernelsLauncher.cuh"
#include "EditKernelsLauncher.cuh"

_DataAccessKernelsLauncher::_DataAccessKernelsLauncher()
{
    _garbageCollectorKernels = std::make_shared<_GarbageCollectorKernelsLauncher>();
    _editKernels = std::make_shared<_EditKernelsLauncher>();
}

void _DataAccessKernelsLauncher::getData(
    GpuSettings const& gpuSettings,
    SimulationData const& data,
    int2 const& rectUpperLeft,
    int2 const& rectLowerRight,
    DataAccessTO const& dataTO)
{
    KERNEL_CALL_1_1(cudaClearDataTO, dataTO);
    KERNEL_CALL(cudaGetCellDataWithoutConnections, rectUpperLeft, rectLowerRight, data, dataTO);
    KERNEL_CALL(cudaResolveConnections, data, dataTO);
    KERNEL_CALL(cudaGetTokenData, data, dataTO);
    KERNEL_CALL(cudaGetParticleData, rectUpperLeft, rectLowerRight, data, dataTO);
}

void _DataAccessKernelsLauncher::getSelectedData(
    GpuSettings const& gpuSettings,
    SimulationData const& data,
    bool includeClusters,
    DataAccessTO const& dataTO)
{
    KERNEL_CALL_1_1(cudaClearDataTO, dataTO);
    KERNEL_CALL(cudaGetSelectedCellDataWithoutConnections, data, includeClusters, dataTO);
    KERNEL_CALL(cudaResolveConnections, data, dataTO);
    KERNEL_CALL(cudaGetTokenData, data, dataTO);
    KERNEL_CALL(cudaGetSelectedParticleData, data, dataTO);
}

void _DataAccessKernelsLauncher::getInspectedData(
    GpuSettings const& gpuSettings,
    SimulationData const& data,
    InspectedEntityIds entityIds,
    DataAccessTO const& dataTO)
{
    KERNEL_CALL_1_1(cudaClearDataTO, dataTO);
    KERNEL_CALL(cudaGetInspectedCellDataWithoutConnections, entityIds, data, dataTO);
    KERNEL_CALL(cudaResolveConnections, data, dataTO);
    KERNEL_CALL(cudaGetTokenData, data, dataTO);
    KERNEL_CALL(cudaGetInspectedParticleData, entityIds, data, dataTO);
}

void _DataAccessKernelsLauncher::getOverlayData(
    GpuSettings const& gpuSettings,
    SimulationData const& data,
    int2 rectUpperLeft,
    int2 rectLowerRight,
    DataAccessTO const& dataTO)
{
    KERNEL_CALL_1_1(cudaClearDataTO, dataTO);
    KERNEL_CALL(cudaGetOverlayData, rectUpperLeft, rectLowerRight, data, dataTO);
}

void _DataAccessKernelsLauncher::addData(GpuSettings const& gpuSettings, SimulationData const& data, DataAccessTO const& dataTO, bool selectData, bool createIds)
{
    KERNEL_CALL_1_1(cudaSaveNumEntries, data);
    KERNEL_CALL(cudaAdaptNumberGenerator, data.numberGen, dataTO);
    KERNEL_CALL(cudaCreateDataFromTO, data, dataTO, selectData, createIds);
    _garbageCollectorKernels->cleanupAfterDataManipulation(gpuSettings, data);
    if (selectData) {
        _editKernels->rolloutSelection(gpuSettings, data);
    }
}

void _DataAccessKernelsLauncher::clearData(GpuSettings const& gpuSettings, SimulationData const& data)
{
    KERNEL_CALL(cudaClearData, data);
}

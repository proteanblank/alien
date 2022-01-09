#pragma once

#include "EngineInterface/Colors.h"
#include "EngineInterface/SimulationParameters.h"
#include "EngineInterface/ShallowUpdateSelectionData.h"

#include "cuda_runtime_api.h"
#include "sm_60_atomic_functions.h"

#include "AccessTOs.cuh"
#include "Base.cuh"
#include "Map.cuh"
#include "EntityFactory.cuh"
#include "GarbageCollectorKernels.cuh"
#include "SelectionResult.cuh"
#include "CellConnectionProcessor.cuh"
#include "CellProcessor.cuh"

#include "SimulationData.cuh"

__global__ void cudaPrepareForUpdate(SimulationData data);
__global__ void cudaConnectSelection(SimulationData data, int* result);
__global__ void cudaUpdateMapForConnection(SimulationData data);
__global__ void cudaUpdateAngleAndAngularVelForSelection(ShallowUpdateSelectionData updateData, SimulationData data, float2 center);
__global__ void cudaCalcAccumulatedCenter(ShallowUpdateSelectionData updateData, SimulationData data, float2* center, int* numEntities);
__global__ void cudaUpdatePosAndVelForSelection(ShallowUpdateSelectionData updateData, SimulationData data);
__global__ void cudaDisconnectSelection(SimulationData data, int* result);
__global__ void cudaProcessConnectionChanges(SimulationData data);
__global__ void cudaExistsSelection(PointSelectionData pointData, SimulationData data, int* result);
__global__ void cudaSetSelection(float2 pos, float radius, SimulationData data);
__global__ void cudaSetSelection(AreaSelectionData selectionData, SimulationData data);
__global__ void cudaRemoveSelection(SimulationData data, bool onlyClusterSelection);
__global__ void cudaSwapSelection(float2 pos, float radius, SimulationData data);
__global__ void cudaRolloutSelectionStep(SimulationData data, int* result);
__global__ void cudaApplyForce(ApplyForceData applyData, SimulationData data);
__global__ void cudaGetSelectionShallowData(SimulationData data, SelectionResult selectionResult);
__global__ void cudaRemoveSelectedEntities(SimulationData data, bool includeClusters);
__global__ void cudaColorSelectedEntities(SimulationData data, unsigned char color, bool includeClusters);
__global__ void cudaChangeSimulationData(SimulationData data, DataAccessTO changeDataTO);

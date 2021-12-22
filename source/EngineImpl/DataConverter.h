#pragma once

#include "EngineInterface/Definitions.h"
#include "EngineInterface/Descriptions.h"
#include "EngineInterface/OverlayDescriptions.h"
#include "EngineInterface/SimulationParameters.h"
#include "EngineGpuKernels/AccessTOs.cuh"
#include "Definitions.h"

#include <unordered_map>

class DataConverter
{
public:
    DataConverter(SimulationParameters const& parameters, GpuSettings const& gpuConstants);

    DataDescription convertAccessTOtoDataDescription(DataAccessTO const& dataTO);
    OverlayDescription convertAccessTOtoOverlayDescription(DataAccessTO const& dataTO);
    void convertDataDescriptionToAccessTO(DataAccessTO& result, DataDescription const& description);

private:
	struct CreateClusterReturnData
    {
        ClusterDescription cluster;
        std::unordered_map<int, int> cellTOIndexToCellDescIndex;
	};
    CreateClusterReturnData scanAndCreateClusterDescription(
        DataAccessTO const& dataTO,
        int startCellIndex,
        std::unordered_set<int>& freeCellIndices) const;
    CellDescription createCellDescription(DataAccessTO const& dataTO, int cellIndex) const;

	void addCell(
        DataAccessTO const& dataTO,
        CellRolloutDescription const& cellToAdd,
        unordered_map<uint64_t, int>& cellIndexTOByIds);
    void addParticle(DataAccessTO const& dataTO, ParticleRolloutDescription const& particleDesc);

	void setConnections(
        DataAccessTO const& dataTO,
        CellRolloutDescription const& cellToAdd,
        unordered_map<uint64_t, int> const& cellIndexByIds);

    int convertStringAndReturnStringIndex(DataAccessTO const& dataTO, std::string const& s);

private:
	SimulationParameters _parameters;
    GpuSettings _gpuConstants;
};

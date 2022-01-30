﻿#include "RenderingKernels.cuh"

namespace
{
    __device__ __inline__ void drawPixel(uint64_t* imageData, unsigned int index, float3 const& color)
    {
        imageData[index] = toUInt64(color.y * 225.0f) << 16 | toUInt64(color.x * 225.0f) << 0 | toUInt64(color.z * 225.0f) << 32;
    }

    __device__ __inline__ void drawAddingPixel(uint64_t* imageData, unsigned int index, float3 const& colorToAdd)
    {
        uint64_t rawColorToAdd = toUInt64(colorToAdd.y * 255.0f) << 16 | toUInt64(colorToAdd.x * 255.0f) << 0 | toUInt64(colorToAdd.z * 255.0f) << 32;
        alienAtomicAdd(&imageData[index], rawColorToAdd);
    }

    __device__ __inline__ float3 colorToFloat3(unsigned int value)
    {
        return float3{toFloat(value & 0xff) / 255, toFloat((value >> 8) & 0xff) / 255, toFloat((value >> 16) & 0xff) / 255};
    }

    __device__ __inline__ float3 mix(float3 const& a, float3 const& b, float factor)
    {
        return float3{a.x * factor + b.x * (1 - factor), a.y * factor + b.y * (1 - factor), a.z * factor + b.z * (1 - factor)};
    }

    __device__ __inline__ float3 mix(float3 const& a, float3 const& b, float3 const& c, float factor1, float factor2)
    {
        float weight1 = factor1 * factor2;
        float weight2 = 1 - factor1;
        float weight3 = 1 - factor2;
        float sum = weight1 + weight2 + weight3;
        weight1 /= sum;
        weight2 /= sum;
        weight3 /= sum;
        return float3{
            a.x * weight1 + b.x * weight2 + c.x * weight3, a.y * weight1 + b.y * weight2 + c.y * weight3, a.z * weight1 + b.z * weight2 + c.z * weight3};
    }

    __device__ __inline__ float2 mapUniversePosToVectorImagePos(float2 const& rectUpperLeft, float2 const& pos, float zoom)
    {
        return float2{(pos.x - rectUpperLeft.x) * zoom, (pos.y - rectUpperLeft.y) * zoom};
    }

    __device__ __inline__ float3 calcColor(Cell* cell, int selected)
    {
        uint32_t cellColor;
        switch (cell->metadata.color % 7) {
        case 0: {
            cellColor = Const::IndividualCellColor1;
            break;
        }
        case 1: {
            cellColor = Const::IndividualCellColor2;
            break;
        }
        case 2: {
            cellColor = Const::IndividualCellColor3;
            break;
        }
        case 3: {
            cellColor = Const::IndividualCellColor4;
            break;
        }
        case 4: {
            cellColor = Const::IndividualCellColor5;
            break;
        }
        case 5: {
            cellColor = Const::IndividualCellColor6;
            break;
        }
        case 6: {
            cellColor = Const::IndividualCellColor7;
            break;
        }
        }

        float factor = min(300.0f, cell->energy) / 320.0f;
        if (1 == selected) {
            factor *= 2.5f;
        }
        if (2 == selected) {
            factor *= 1.75f;
        }

        return {
            toFloat((cellColor >> 16) & 0xff) / 256.0f * factor,
            toFloat((cellColor >> 8) & 0xff) / 256.0f * factor,
            toFloat(cellColor & 0xff) / 256.0f * factor};
    }

    __device__ __inline__ float3 calcColor(Particle* particle, bool selected)
    {
        auto intensity = max(min((toInt(particle->energy) + 10) * 5, 150), 20) / 266.0f;
        if (selected) {
            intensity *= 2.5f;
        }

        return {intensity, 0, 0.08f};
    }

    __device__ __inline__ float3 calcColor(bool selected) { return selected ? float3{0.75f, 0.75f, 0.75f} : float3{0.5f, 0.5f, 0.5f}; }

    __device__ __inline__ void drawDot(uint64_t* imageData, int2 const& imageSize, float2 const& pos, float3 const& colorToAdd)
    {
        int2 intPos{toInt(pos.x), toInt(pos.y)};
        if (intPos.x >= 1 && intPos.x < imageSize.x - 1 && intPos.y >= 1 && intPos.y < imageSize.y - 1) {

            float2 posFrac{pos.x - intPos.x, pos.y - intPos.y};
            unsigned int index = intPos.x + intPos.y * imageSize.x;

            float3 colorToAdd1 = colorToAdd * (1.0f - posFrac.x) * (1.0f - posFrac.y);
            drawAddingPixel(imageData, index, colorToAdd1);

            float3 colorToAdd2 = colorToAdd * posFrac.x * (1.0f - posFrac.y);
            drawAddingPixel(imageData, index, colorToAdd2);

            float3 colorToAdd3 = colorToAdd * (1.0f - posFrac.x) * posFrac.y;
            drawAddingPixel(imageData, index + imageSize.x, colorToAdd3);

            float3 colorToAdd4 = colorToAdd * posFrac.x * posFrac.y;
            drawAddingPixel(imageData, index + imageSize.x + 1, colorToAdd4);
        }
    }

    __device__ __inline__ void drawCircle(uint64_t* imageData, int2 const& imageSize, float2 pos, float3 color, float radius, bool inverted = false)
    {
        if (radius > 1.5 - FP_PRECISION) {
            auto radiusSquared = radius * radius;
            for (float x = -radius; x <= radius; x += 1.0f) {
                for (float y = -radius; y <= radius; y += 1.0f) {
                    auto rSquared = x * x + y * y;
                    if (rSquared <= radiusSquared) {
                        auto factor = inverted ? (rSquared / radiusSquared) * 2 : (1.0f - rSquared / radiusSquared) * 2;
                        drawDot(imageData, imageSize, pos + float2{x, y}, color * min(factor, 1.0f));
                    }
                }
            }
        } else {
            color = color * radius * 2;
            drawDot(imageData, imageSize, pos, color);
            color = color * 0.3f;
            drawDot(imageData, imageSize, pos + float2{1, 0}, color);
            drawDot(imageData, imageSize, pos + float2{-1, 0}, color);
            drawDot(imageData, imageSize, pos + float2{0, 1}, color);
            drawDot(imageData, imageSize, pos + float2{0, -1}, color);
        }
    }

    __device__ __inline__ void
    drawLine(float2 const& start, float2 const& end, float3 const& color, uint64_t* imageData, int2 imageSize, float pixelDistance = 1.5f)
    {
        float dist = Math::length(end - start);
        float2 const v = {static_cast<float>(end.x - start.x) / dist * pixelDistance, static_cast<float>(end.y - start.y) / dist * pixelDistance};
        float2 pos = start;

        for (float d = 0; d <= dist; d += pixelDistance) {
            drawDot(imageData, imageSize, pos, color);
            pos = pos + v;
        }
    }
}

/************************************************************************/
/* Main      															*/
/************************************************************************/
__global__ void cudaDrawBackground(uint64_t* imageData, int2 imageSize, int2 worldSize, float zoom, float2 rectUpperLeft, float2 rectLowerRight)
{
    int2 outsideRectUpperLeft{-min(toInt(rectUpperLeft.x * zoom), 0), -min(toInt(rectUpperLeft.y * zoom), 0)};
    int2 outsideRectLowerRight{
        imageSize.x - max(toInt((rectLowerRight.x - worldSize.x) * zoom), 0), imageSize.y - max(toInt((rectLowerRight.y - worldSize.y) * zoom), 0)};

    MapInfo map;
    map.init(worldSize);
    auto spaceColor = colorToFloat3(Const::SpaceColor);
    auto spotColor1 = colorToFloat3(cudaSimulationParametersSpots.spots[0].color);
    auto spotColor2 = colorToFloat3(cudaSimulationParametersSpots.spots[1].color);

    auto const block = calcPartition(imageSize.x * imageSize.y, threadIdx.x + blockIdx.x * blockDim.x, blockDim.x * gridDim.x);
    for (int index = block.startIndex; index <= block.endIndex; ++index) {
        auto x = index % imageSize.x;
        auto y = index / imageSize.x;
        if (x < outsideRectUpperLeft.x || y < outsideRectUpperLeft.y || x >= outsideRectLowerRight.x || y >= outsideRectLowerRight.y) {
            imageData[index] = 0;
        } else {
            if (0 == cudaSimulationParametersSpots.numSpots) {
                drawPixel(imageData, index, spaceColor);
            }
            if (1 == cudaSimulationParametersSpots.numSpots) {
                float2 worldPos = {toFloat(x) / zoom + rectUpperLeft.x, toFloat(y) / zoom + rectUpperLeft.y};
                auto distance = map.mapDistance(worldPos, {cudaSimulationParametersSpots.spots[0].posX, cudaSimulationParametersSpots.spots[0].posY});
                auto coreRadius = cudaSimulationParametersSpots.spots[0].coreRadius;
                auto fadeoutRadius = cudaSimulationParametersSpots.spots[0].fadeoutRadius + 1;
                auto factor = distance < coreRadius ? 0.0f : min(1.0f, (distance - coreRadius) / fadeoutRadius);
                auto resultingColor = mix(spaceColor, spotColor1, factor);
                drawPixel(imageData, index, resultingColor);
            }
            if (2 == cudaSimulationParametersSpots.numSpots) {
                float2 worldPos = {toFloat(x) / zoom + rectUpperLeft.x, toFloat(y) / zoom + rectUpperLeft.y};
                auto distance1 = map.mapDistance(worldPos, {cudaSimulationParametersSpots.spots[0].posX, cudaSimulationParametersSpots.spots[0].posY});
                auto distance2 = map.mapDistance(worldPos, {cudaSimulationParametersSpots.spots[1].posX, cudaSimulationParametersSpots.spots[1].posY});

                auto coreRadius1 = cudaSimulationParametersSpots.spots[0].coreRadius;
                auto fadeoutRadius1 = cudaSimulationParametersSpots.spots[0].fadeoutRadius + 1;
                auto factor1 = distance1 < coreRadius1 ? 0.0f : min(1.0f, (distance1 - coreRadius1) / fadeoutRadius1);
                auto coreRadius2 = cudaSimulationParametersSpots.spots[1].coreRadius;
                auto fadeoutRadius2 = cudaSimulationParametersSpots.spots[1].fadeoutRadius + 1;
                auto factor2 = distance2 < coreRadius2 ? 0.0f : min(1.0f, (distance2 - coreRadius2) / fadeoutRadius2);

                auto resultingColor = mix(spaceColor, spotColor1, spotColor2, factor1, factor2);
                drawPixel(imageData, index, resultingColor);
            }
        }
    }
}

__global__ void cudaDrawCells(int2 universeSize, float2 rectUpperLeft, float2 rectLowerRight, Array<Cell*> cells, uint64_t* imageData, int2 imageSize, float zoom)
{
    auto const partition = calcPartition(cells.getNumEntries(), threadIdx.x + blockIdx.x * blockDim.x, blockDim.x * gridDim.x);

    MapInfo map;
    map.init(universeSize);

    for (int index = partition.startIndex; index <= partition.endIndex; ++index) {
        auto const& cell = cells.at(index);

        auto cellPos = cell->absPos;
        map.mapPosCorrection(cellPos);
        if (isContainedInRect(rectUpperLeft, rectLowerRight, cellPos)) {
            auto cellImagePos = mapUniversePosToVectorImagePos(rectUpperLeft, cellPos, zoom);
            auto color = calcColor(cell, cell->selected);
            auto radius = 1 == cell->selected ? zoom / 2 : zoom / 3;
            drawCircle(imageData, imageSize, cellImagePos, color, radius, true);
            color = color * min((zoom - 1.0f) / 3, 1.0f);

            //draw connection lines
            if (zoom >= 1.0f) {
                for (int i = 0; i < cell->numConnections; ++i) {
                    auto const otherCell = cell->connections[i].cell;
                    auto const otherCellPos = otherCell->absPos;
                    auto topologyCorrection = map.correctionIncrement(cellPos, otherCellPos);
                    if (Math::lengthSquared(topologyCorrection) < FP_PRECISION) {
                        auto const otherCellImagePos = mapUniversePosToVectorImagePos(rectUpperLeft, otherCellPos, zoom);
                        drawLine(cellImagePos, otherCellImagePos, color, imageData, imageSize);
                    }
                }
            }

            //draw arrows
            if (zoom >= 15.0f) {
                for (int i = 0; i < cell->numConnections; ++i) {
                    auto const otherCell = cell->connections[i].cell;
                    auto const otherCellPos = otherCell->absPos;
                    auto topologyCorrection = map.correctionIncrement(cellPos, otherCellPos);
                    if (Math::lengthSquared(topologyCorrection) > FP_PRECISION) {
                        continue;
                    }
                    if ((cell->branchNumber + 1 - otherCell->branchNumber) % cudaSimulationParameters.cellMaxTokenBranchNumber == 0) {
                        auto const arrowEnd =
                            mapUniversePosToVectorImagePos(rectUpperLeft, otherCellPos + Math::normalized(cellPos - otherCellPos) / 3, zoom);
                        auto direction = Math::normalized(arrowEnd - cellImagePos);
                        {
                            float2 arrowPartStart = {-direction.x + direction.y, -direction.x - direction.y};
                            arrowPartStart = arrowPartStart * zoom / 6 + arrowEnd;
                            drawLine(arrowPartStart, arrowEnd, color, imageData, imageSize, 0.7f);
                        }
                        {
                            float2 arrowPartStart = {-direction.x - direction.y, direction.x - direction.y};
                            arrowPartStart = arrowPartStart * zoom / 6 + arrowEnd;
                            drawLine(arrowPartStart, arrowEnd, color, imageData, imageSize, 0.7f);
                        }
                    }
                    if ((cell->branchNumber - 1 - otherCell->branchNumber) % cudaSimulationParameters.cellMaxTokenBranchNumber == 0) {
                        auto const arrowEnd = mapUniversePosToVectorImagePos(rectUpperLeft, cellPos + Math::normalized(otherCellPos - cellPos) / 3, zoom);
                        auto const otherCellImagePos = mapUniversePosToVectorImagePos(rectUpperLeft, otherCellPos, zoom);
                        auto direction = Math::normalized(arrowEnd - otherCellImagePos);
                        {
                            float2 arrowPartStart = {-direction.x + direction.y, -direction.x - direction.y};
                            arrowPartStart = arrowPartStart * zoom / 6 + arrowEnd;
                            drawLine(arrowPartStart, arrowEnd, color, imageData, imageSize, 0.7f);
                        }
                        {
                            float2 arrowPartStart = {-direction.x - direction.y, direction.x - direction.y};
                            arrowPartStart = arrowPartStart * zoom / 6 + arrowEnd;
                            drawLine(arrowPartStart, arrowEnd, color, imageData, imageSize, 0.7f);
                        }
                    }
                }
            }
        }
    }
}

__global__ void
cudaDrawTokens(int2 universeSize, float2 rectUpperLeft, float2 rectLowerRight, Array<Token*> tokens, uint64_t* imageData, int2 imageSize, float zoom)
{
    MapInfo map;
    map.init(universeSize);

    auto partition = calcPartition(tokens.getNumEntries(), threadIdx.x + blockIdx.x * blockDim.x, blockDim.x * gridDim.x);
    for (auto tokenIndex = partition.startIndex; tokenIndex <= partition.endIndex; ++tokenIndex) {
        auto const& token = tokens.at(tokenIndex);
        auto const& cell = token->cell;

        auto cellPos = cell->absPos;
        map.mapPosCorrection(cellPos);
        auto const cellImagePos = mapUniversePosToVectorImagePos(rectUpperLeft, cellPos, zoom);
        if (isContainedInRect({0, 0}, imageSize, cellImagePos)) {
            auto const color = calcColor(false);
            drawCircle(imageData, imageSize, cellImagePos, color, zoom / 2);
        }
    }
}

__global__ void
cudaDrawParticles(int2 universeSize, float2 rectUpperLeft, float2 rectLowerRight, Array<Particle*> particles, uint64_t* imageData, int2 imageSize, float zoom)
{
    auto const particleBlock = calcPartition(particles.getNumEntries(), threadIdx.x + blockIdx.x * blockDim.x, blockDim.x * gridDim.x);

    for (int index = particleBlock.startIndex; index <= particleBlock.endIndex; ++index) {
        auto const& particle = particles.at(index);

        auto const particleImagePos = mapUniversePosToVectorImagePos(rectUpperLeft, particle->absPos, zoom);
        if (isContainedInRect({0, 0}, imageSize, particleImagePos)) {
            auto const color = calcColor(particle, 0 != particle->selected);
            auto radius = 1 == particle->selected ? zoom / 2 : zoom / 3;
            drawCircle(imageData, imageSize, particleImagePos, color, radius);
        }
    }
}

__global__ void cudaDrawFlowCenters(uint64_t* targetImage, float2 rectUpperLeft, int2 imageSize, float zoom)
{
    if (cudaFlowFieldSettings.active) {
        for (int i = 0; i < cudaFlowFieldSettings.numCenters; ++i) {
            auto const& radialFlowData = cudaFlowFieldSettings.centers[i];
            int screenPosX = toInt(radialFlowData.posX * zoom) - rectUpperLeft.x * zoom;
            int screenPosY = toInt(radialFlowData.posY * zoom) - rectUpperLeft.y * zoom;
            auto drawX = screenPosX;
            auto drawY = screenPosY;
            if (0 <= drawX && drawX < imageSize.x && 0 <= drawY && drawY < imageSize.y) {
                int index = drawX + drawY * imageSize.x;
                targetImage[index] = 0xffff00000000;
            }
        }
    }
}

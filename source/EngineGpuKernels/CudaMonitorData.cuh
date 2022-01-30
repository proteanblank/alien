#pragma once

#include "EngineInterface/OverallStatistics.h"

#include "Base.cuh"
#include "Definitions.cuh"
#include "Entities.cuh"

class CudaMonitorData
{
public:
    __host__ void init()
    {
        CudaMemoryManager::getInstance().acquireMemory<int>(1, _numCells);
        CudaMemoryManager::getInstance().acquireMemory<int>(1, _numTokens);
        CudaMemoryManager::getInstance().acquireMemory<int>(1, _numParticles);
        CudaMemoryManager::getInstance().acquireMemory<double>(1, _internalEnergy);

        CHECK_FOR_CUDA_ERROR(cudaMemset(_numCells, 0, sizeof(int)));
        CHECK_FOR_CUDA_ERROR(cudaMemset(_numTokens, 0, sizeof(int)));
        CHECK_FOR_CUDA_ERROR(cudaMemset(_numParticles, 0, sizeof(int)));

        double zero = 0.0;
        CHECK_FOR_CUDA_ERROR(cudaMemcpy(_internalEnergy, &zero, sizeof(double), cudaMemcpyHostToDevice));

    }

    __host__ void free()
    {
        CudaMemoryManager::getInstance().freeMemory(_numCells);
        CudaMemoryManager::getInstance().freeMemory(_numTokens);
        CudaMemoryManager::getInstance().freeMemory(_numParticles);
        CudaMemoryManager::getInstance().freeMemory(_internalEnergy);
    }

    struct MonitorData
    {
        uint64_t timeStep = 0;
        int numCells = 0;
        int numParticles = 0;
        int numTokens = 0;
        double totalInternalEnergy = 0.0;
    };
    __host__ MonitorData getMonitorData(uint64_t timeStep)
    {
        MonitorData result;
        CHECK_FOR_CUDA_ERROR(cudaMemcpy(&result.numCells, _numCells, sizeof(int), cudaMemcpyDeviceToHost));
        CHECK_FOR_CUDA_ERROR(cudaMemcpy(&result.numParticles, _numParticles, sizeof(int), cudaMemcpyDeviceToHost));
        CHECK_FOR_CUDA_ERROR(cudaMemcpy(&result.numTokens, _numTokens, sizeof(int), cudaMemcpyDeviceToHost));
        CHECK_FOR_CUDA_ERROR(cudaMemcpy(&result.totalInternalEnergy, _internalEnergy, sizeof(double), cudaMemcpyDeviceToHost));
        result.timeStep = timeStep;
        return result;
    }

    __inline__ __device__ void reset()
    {
        *_numCells = 0;
        *_numTokens = 0;
        *_numParticles = 0;
        *_internalEnergy = 0.0f;
    }

    __inline__ __device__ void setNumCells(int value) { *_numCells = value; }
    __inline__ __device__ void setNumParticles(int value) { *_numParticles = value; }
    __inline__ __device__ void setNumTokens(int value) { *_numTokens = value; }


/*
    __inline__ __device__ void incInternalEnergy(float changeValue)
    {
        atomicAdd(_internalEnergy, static_cast<double>(changeValue));
    }
*/

private:
    int* _numCells;
    int* _numTokens;
    int* _numParticles;
    double* _internalEnergy;
};


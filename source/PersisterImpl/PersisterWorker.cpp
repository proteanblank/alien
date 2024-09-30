#include "PersisterWorker.h"

#include <algorithm>

#include "EngineInterface/SerializerService.h"
#include "EngineInterface/SimulationController.h"

_PersisterWorker::_PersisterWorker(SimulationController const& simController)
    : _simController(simController)
{
}

void _PersisterWorker::runThreadLoop()
{
    try {
        std::unique_lock lock(_jobMutex);
        while (!_isShutdown.load()) {
            _conditionVariable.wait(lock);
            processJobs(lock);
        }
    } catch (std::exception const&) {
        //#TODO
    }
}

void _PersisterWorker::shutdown()
{
    _isShutdown = true;
    _conditionVariable.notify_all();
}

bool _PersisterWorker::isBusy() const
{
    std::unique_lock uniqueLock(_jobMutex);
    return !_openJobs.empty() || !_inProgressJobs.empty();
}

PersisterJobState _PersisterWorker::getJobState(PersisterJobId const& id) const
{
    std::unique_lock uniqueLock(_jobMutex);
    if (std::ranges::find_if(_openJobs, [&](PersisterJob const& job) { return job->getId() == id; }) != _openJobs.end()) {
        return PersisterJobState::InQueue;
    }
    if (std::ranges::find_if(_inProgressJobs, [&](PersisterJob const& job) { return job->getId() == id; }) != _inProgressJobs.end()) {
        return PersisterJobState::InProgress;
    }
    if (std::ranges::find_if(_finishedJobs, [&](PersisterJobResult const& job) { return job->getId() == id; }) != _finishedJobs.end()) {
        return PersisterJobState::Finished;
    }
    THROW_NOT_IMPLEMENTED();
}

PersisterJobResult _PersisterWorker::fetchJobResult(PersisterJobId const& id)
{
    std::unique_lock uniqueLock(_jobMutex);

    auto findResult = std::ranges::find_if(_finishedJobs, [&](PersisterJobResult const& job) { return job->getId() == id; });

    if (findResult != _finishedJobs.end()) {
        auto resultCopy = *findResult;
        _finishedJobs.erase(findResult);
        return resultCopy;
    }
    THROW_NOT_IMPLEMENTED();
}

void _PersisterWorker::addJob(PersisterJob const& job)
{
    {
        std::unique_lock uniqueLock(_jobMutex);
        _openJobs.emplace_back(job);
    }
    _conditionVariable.notify_all();
}

void _PersisterWorker::processJobs(std::unique_lock<std::mutex>& lock)
{
    if (_openJobs.empty()) {
        return;
    }

    while (!_openJobs.empty()) {

        auto job = _openJobs.front();
        _openJobs.pop_front();

        if (auto const& saveToDiscJob = std::dynamic_pointer_cast<_SaveToDiscJob>(job)) {
            _inProgressJobs.push_back(job);
            auto jobResult = processSaveToDiscJob(lock, saveToDiscJob);
            auto findResult = std::ranges::find_if(_inProgressJobs, [&](PersisterJob const& otherJob) { return otherJob->getId() == job->getId(); });
            _inProgressJobs.erase(findResult);

            _finishedJobs.emplace_back(jobResult);
        }

    }
}

PersisterJobResult _PersisterWorker::processSaveToDiscJob(std::unique_lock<std::mutex>& lock, SaveToDiscJob const& job)
{
    lock.unlock();

    DeserializedSimulation deserializedData;
    deserializedData.auxiliaryData.timestep = static_cast<uint32_t>(_simController->getCurrentTimestep());
    deserializedData.auxiliaryData.realTime = _simController->getRealTime();
    deserializedData.auxiliaryData.zoom = job->getZoom();
    deserializedData.auxiliaryData.center = job->getCenter();
    deserializedData.auxiliaryData.generalSettings = _simController->getGeneralSettings();
    deserializedData.auxiliaryData.simulationParameters = _simController->getSimulationParameters();
    deserializedData.statistics = _simController->getStatisticsHistory().getCopiedData();
    deserializedData.mainData = _simController->getClusteredSimulationData();

    SerializerService::serializeSimulationToFiles(job->getFilename(), deserializedData);

    lock.lock();

    return std::make_shared<_SaveToDiscJobResult>(job->getId(), deserializedData.auxiliaryData.timestep, deserializedData.auxiliaryData.realTime);
}

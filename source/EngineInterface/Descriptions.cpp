#include "Descriptions.h"

#include <boost/range/adaptors.hpp>

#include "Base/Math.h"
#include "Base/Physics.h"

CellDescription& CellDescription::addToken(TokenDescription const& value)
{
    tokens.emplace_back(value);
    return *this;
}

CellDescription& CellDescription::addToken(int index, TokenDescription const& value)
{
    tokens.insert(tokens.begin() + index, value);
    return *this;
}

CellDescription& CellDescription::delToken(int index)
{
    tokens.erase(tokens.begin() + index);
    return *this;
}

bool CellDescription::isConnectedTo(uint64_t id) const
{
    return std::find_if(
               connections.begin(),
               connections.end(),
               [&id](auto const& connection) { return connection.cellId == id; })
        != connections.end();
}

RealVector2D ClusterDescription::getClusterPosFromCells() const
{
    RealVector2D result;
    for (auto const& cell : cells) {
        result += cell.pos;
    }
    result /= cells.size();
    return result;
}

void ClusteredDataDescription::setCenter(RealVector2D const& center)
{
    auto origCenter = calcCenter();
    auto delta = center - origCenter;
    shift(delta);
}

RealVector2D ClusteredDataDescription::calcCenter() const
{
    RealVector2D result;
    int numEntities = 0;
    for (auto const& cluster : clusters) {
        for (auto const& cell : cluster.cells) {
            result += cell.pos;
            ++numEntities;
        }
    }
    for (auto const& particle : particles) {
        result += particle.pos;
        ++numEntities;
    }
    result /= numEntities;
    return result;
}

void ClusteredDataDescription::shift(RealVector2D const& delta)
{
    for (auto& cluster : clusters) {
        for (auto& cell : cluster.cells) {
            cell.pos += delta;
        }
    }
    for (auto& particle : particles) {
        particle.pos += delta;
    }
}

DataDescription::DataDescription(ClusteredDataDescription const& clusteredData)
{
    for (auto const& cluster : clusteredData.clusters) {
        addCells(cluster.cells);
    }
    particles = clusteredData.particles;
}

DataDescription& DataDescription::add(DataDescription const& other)
{
    cells.insert(cells.end(), other.cells.begin(), other.cells.end());
    particles.insert(particles.end(), other.particles.begin(), other.particles.end());
    return *this;
}

DataDescription& DataDescription::addCells(std::vector<CellDescription> const& value)
{
    cells.insert(cells.end(), value.begin(), value.end());
    return *this;
}

DataDescription& DataDescription::addCell(CellDescription const& value)
{
    addCells({value});
    return *this;
}

DataDescription& DataDescription::addParticles(std::vector<ParticleDescription> const& value)
{
    particles.insert(particles.end(), value.begin(), value.end());
    return *this;
}

DataDescription& DataDescription::addParticle(ParticleDescription const& value)
{
    addParticles({value});
    return *this;
}

void DataDescription::clear()
{
    cells.clear();
    particles.clear();
}

bool DataDescription::isEmpty() const
{
    if (!cells.empty()) {
        return false;
    }
    if (!particles.empty()) {
        return false;
    }
    return true;
}

void DataDescription::setCenter(RealVector2D const& center)
{
    auto origCenter = calcCenter();
    auto delta = center - origCenter;
    shift(delta);
}

RealVector2D DataDescription::calcCenter() const
{
    RealVector2D result;
    auto numEntities = cells.size() + particles.size();
    for (auto const& cell : cells) {
        result += cell.pos;
    }
    for (auto const& particle : particles) {
        result += particle.pos;
    }
    result /= numEntities;
    return result;
}

void DataDescription::shift(RealVector2D const& delta)
{
    for (auto& cell : cells) {
        cell.pos += delta;
    }
    for (auto& particle : particles) {
        particle.pos += delta;
    }
}

void DataDescription::rotate(float angle)
{
    auto rotationMatrix = Math::calcRotationMatrix(angle);
    auto center = calcCenter();

    auto rotate = [&](RealVector2D& pos) {
        auto relPos = pos - center;
        auto rotatedRelPos = rotationMatrix * relPos;
        pos = center + rotatedRelPos;
    };
    for (auto& cell : cells) {
        rotate(cell.pos);
    }
    for (auto& particle : particles) {
        rotate(particle.pos);
    }
}

void DataDescription::accelerate(RealVector2D const& velDelta, float angularVelDelta)
{
    auto center = calcCenter();

    auto accelerate = [&](RealVector2D const& pos, RealVector2D& vel) {
        auto relPos = pos - center;
        vel += Physics::tangentialVelocity(relPos, velDelta, angularVelDelta);
    };
    for (auto& cell : cells) {
        accelerate(cell.pos, cell.vel);
    }
    for (auto& particle : particles) {
        accelerate(particle.pos, particle.vel);
    }
}

DataDescription&
DataDescription::addConnection(uint64_t const& cellId1, uint64_t const& cellId2, std::unordered_map<uint64_t, int>& cache)
{
    auto& cell1 = getCellRef(cellId1, cache);
    auto& cell2 = getCellRef(cellId2, cache);

    auto addConnection = [this, &cache](auto& cell, auto& otherCell) {
        CHECK(cell.connections.size() < cell.maxConnections);

        auto newAngle = Math::angleOfVector(otherCell.pos - cell.pos);

        if (cell.connections.empty()) {
            ConnectionDescription newConnection;
            newConnection.cellId = otherCell.id;
            newConnection.distance = toFloat(Math::length(otherCell.pos - cell.pos));
            newConnection.angleFromPrevious = 360.0;
            cell.connections.emplace_back(newConnection);
            return;
        }
        if (1 == cell.connections.size()) {
            ConnectionDescription newConnection;
            newConnection.cellId = otherCell.id;
            newConnection.distance = toFloat(Math::length(otherCell.pos - cell.pos));

            auto connectedCell = getCellRef(cell.connections.front().cellId, cache);
            auto connectedCellDelta = connectedCell.pos - cell.pos;
            auto prevAngle = Math::angleOfVector(connectedCellDelta);
            auto angleDiff = newAngle - prevAngle;
            if (angleDiff >= 0) {
                newConnection.angleFromPrevious = toFloat(angleDiff);
                cell.connections.begin()->angleFromPrevious = 360.0f - toFloat(angleDiff);
            } else {
                newConnection.angleFromPrevious = 360.0f + toFloat(angleDiff);
                cell.connections.begin()->angleFromPrevious = toFloat(-angleDiff);
            }
            cell.connections.emplace_back(newConnection);
            return;
        }

        auto firstConnectedCell = getCellRef(cell.connections.front().cellId, cache);
        auto firstConnectedCellDelta = firstConnectedCell.pos - cell.pos;
        auto angle = Math::angleOfVector(firstConnectedCellDelta);
        auto connectionIt = ++cell.connections.begin();
        while (true) {
            auto nextAngle = angle + connectionIt->angleFromPrevious;

            if ((angle < newAngle && newAngle <= nextAngle) || (angle < (newAngle + 360.0f) && (newAngle + 360.0f) <= nextAngle)) {
                break;
            }

            ++connectionIt;
            if (connectionIt == cell.connections.end()) {
                connectionIt = cell.connections.begin();
            }
            angle = nextAngle;
            if (angle > 360.0f) {
                angle -= 360.0f;
            }
        }

        ConnectionDescription newConnection;
        newConnection.cellId = otherCell.id;
        newConnection.distance = toFloat(Math::length(otherCell.pos - cell.pos));

        auto angleDiff1 = newAngle - angle;
        if (angleDiff1 < 0) {
            angleDiff1 += 360.0f;
        }
        auto angleDiff2 = connectionIt->angleFromPrevious;

        auto factor = (angleDiff2 != 0) ? angleDiff1 / angleDiff2 : 0.5f;
        newConnection.angleFromPrevious = toFloat(angleDiff2 * factor);
        connectionIt = cell.connections.insert(connectionIt, newConnection);
        ++connectionIt;
        if (connectionIt == cell.connections.end()) {
            connectionIt = cell.connections.begin();
        }
        connectionIt->angleFromPrevious = toFloat(angleDiff2 * (1 - factor));
    };

    addConnection(cell1, cell2);
    addConnection(cell2, cell1);

    return *this;
}

CellDescription& DataDescription::getCellRef(uint64_t const& cellId, std::unordered_map<uint64_t, int>& cache)
{
    auto findResult = cache.find(cellId);
    if (findResult != cache.end()) {
        return cells.at(findResult->second);
    }
    for (int i = 0; i < cells.size(); ++i) {
        auto& cell = cells.at(i);
        if (cell.id == cellId) {
            cache.emplace(cellId, i);
            return cell;
        }
    }
    THROW_NOT_IMPLEMENTED();
}

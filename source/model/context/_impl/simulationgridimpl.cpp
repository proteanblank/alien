#include "simulationgridimpl.h"

SimulationGridImpl::SimulationGridImpl(QObject * parent)
	: SimulationGrid(parent)
{
}

void SimulationGridImpl::init(IntVector2D gridSize)
{
	_size = gridSize;
	for (int x = 0; x < gridSize.x; ++x) {
		_units.push_back(std::vector<SimulationUnit*>(gridSize.y, nullptr));
	}
}

void SimulationGridImpl::registerUnit(IntVector2D gridPos, SimulationUnit * unit)
{
	_units[gridPos.x][gridPos.y] = unit;
}

IntVector2D SimulationGridImpl::getSize() const
{
	return _size;
}

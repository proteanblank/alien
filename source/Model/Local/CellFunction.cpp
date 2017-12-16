#include <QString>
#include <QtCore/qmath.h>

#include "Model/Api/SimulationContext.h"
#include "Model/Api/SimulationParameters.h"
#include "Model/Local/Cell.h"
#include "Model/Local/Physics.h"
#include "Model/Local/UnitContext.h"
#include "Model/Local/SpacePropertiesLocal.h"

#include "CellFunction.h"

QByteArray CellFunction::getInternalData() const
{
	int memorySize = _context->getSimulationParameters()->cellFunctionComputerCellMemorySize;
	return QByteArray(memorySize, 0);
}

void CellFunction::appendDescriptionImpl(CellFeatureDescription & desc) const
{
	desc.setType(getType());
	desc.setVolatileData(getInternalData());
}

qreal CellFunction::calcAngle (Cell* origin, Cell* ref1, Cell* ref2) const
{
	SpacePropertiesLocal* topo = _context->getSpaceProperties();
    QVector2D v1 = topo->displacement(origin->calcPosition(), ref1->calcPosition());
    QVector2D v2 = topo->displacement(origin->calcPosition(), ref2->calcPosition());
    return Physics::clockwiseAngleFromFirstToSecondVector(v1, v2);
}



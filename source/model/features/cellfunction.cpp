#include <QString>
#include <QtCore/qmath.h>

#include "model/entities/grid.h"
#include "model/physics/physics.h"
#include "model/simulationcontext.h"
#include "model/topology.h"

#include "cellfunction.h"


qreal CellFunction::calcAngle (Cell* origin, Cell* ref1, Cell* ref2) const
{
	Topology* topo = _context->getTopology();
    QVector3D v1 = topo->displacement(origin->calcPosition(), ref1->calcPosition());
    QVector3D v2 = topo->displacement(origin->calcPosition(), ref2->calcPosition());
    return Physics::clockwiseAngleFromFirstToSecondVector(v1, v2);
}



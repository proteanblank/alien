#pragma once

#include <QWidget>
#include <QVector2D>

#include "EngineInterface/Definitions.h"
#include "Definitions.h"

namespace Ui {
	class SimulationViewWidget;
}

class SimulationViewWidget : public QWidget
{
    Q_OBJECT
public:
    SimulationViewWidget(QWidget *parent = 0);
    virtual ~SimulationViewWidget();

	void init(Notifier* notifier, SimulationController* controller, SimulationAccess* access, DataRepository* manipulator);
    
    void connectView();
    void disconnectView();
    void refresh();

    ActiveView getActiveView() const;
    void setActiveScene(ActiveView activeScene);

    double getZoomFactor();
    void setZoomFactor(double factor);
    void setZoomFactor(double factor, IntVector2D const& viewPos);

	QVector2D getViewCenterWithIncrement ();

	void toggleCenterSelection(bool value);

    Q_SIGNAL void continuousZoomIn(IntVector2D const& viewPos);
    Q_SIGNAL void continuousZoomOut(IntVector2D const& viewPos);
    Q_SIGNAL void endContinuousZoom();
    Q_SIGNAL void zoomFactorChanged(double factor);

private:
    UniverseView* getActiveUniverseView() const;
    UniverseView* getView(ActiveView activeView) const;

    Ui::SimulationViewWidget *ui;

	SimulationController* _controller = nullptr;

    OpenGLUniverseView* _openGLUniverse = nullptr;
    ItemUniverseView* _itemUniverse = nullptr;

    qreal _posIncrement = 0.0;
};






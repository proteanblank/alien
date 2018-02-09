﻿#include <iostream>
#include <fstream>

#include "Base/GlobalFactory.h"
#include "Base/ServiceLocator.h"
#include "Base/NumberGenerator.h"

#include "Model/Api/ModelBuilderFacade.h"
#include "Model/Api/SimulationController.h"
#include "Model/Api/SimulationContext.h"
#include "Model/Api/SpaceProperties.h"
#include "Model/Api/SimulationAccess.h"
#include "Model/Api/SimulationParameters.h"
#include "Model/Api/Serializer.h"
#include "Model/Api/DescriptionHelper.h"

#include "VersionController.h"
#include "SerializationHelper.h"
#include "InfoController.h"
#include "MainController.h"
#include "MainView.h"
#include "MainModel.h"
#include "DataRepository.h"
#include "Notifier.h"

MainController::MainController(QObject * parent)
	: QObject(parent)
{
}

MainController::~MainController()
{
	delete _view;
}

void MainController::init()
{
	_model = new MainModel(this);
	_view = new MainView();

	auto factory = ServiceLocator::getInstance().getService<GlobalFactory>();
	auto numberGenerator = factory->buildRandomNumberGenerator();

	auto facade = ServiceLocator::getInstance().getService<ModelBuilderFacade>();
	auto serializer = facade->buildSerializer();
	auto simAccessForDataController = facade->buildSimulationAccess();
	auto descHelper = facade->buildDescriptionHelper();
	auto versionController = new VersionController();
	SET_CHILD(_serializer, serializer);
	SET_CHILD(_simAccess, simAccessForDataController);
	SET_CHILD(_descHelper, descHelper);
	SET_CHILD(_versionController, versionController);
	SET_CHILD(_numberGenerator, numberGenerator);
	_repository = new DataRepository(this);
	_notifier = new Notifier(this);

	connect(_serializer, &Serializer::serializationFinished, this, &MainController::serializationFinished);

	_serializer->init();
	_numberGenerator->init(12315312, 0);
	_view->init(_model, this, _serializer, _repository, _notifier, _numberGenerator);

	
	//default simulation
	NewSimulationConfig config{
		8, { 12, 6 },{ 12 * 33 * 3 , 12 * 17 * 3 },
		facade->buildDefaultSymbolTable(),
		facade->buildDefaultSimulationParameters(),
		20000 * 9 * 20
	};
	onNewSimulation(config);
}

void MainController::onRunSimulation(bool run)
{
	_simController->setRun(run);
	_versionController->clearStack();
}

void MainController::onStepForward()
{
	_versionController->saveSimulationContentToStack();
	_simController->calculateSingleTimestep();
}

void MainController::onStepBackward(bool& emptyStack)
{
	_versionController->loadSimulationContentFromStack();
	emptyStack = _versionController->isStackEmpty();
}

void MainController::onMakeSnapshot()
{
	_versionController->makeSnapshot();
}

void MainController::onRestoreSnapshot()
{
	_versionController->restoreSnapshot();
}

void MainController::onNewSimulation(NewSimulationConfig config)
{
	delete _simController;

	_model->setSimulationParameters(config.parameters);
	_model->setSymbolTable(config.symbolTable);

	auto facade = ServiceLocator::getInstance().getService<ModelBuilderFacade>();
	_simController = facade->buildSimulationController(config.maxThreads, config.gridSize, config.universeSize, config.symbolTable, config.parameters);
	connectSimController();
	_simAccess->init(_simController->getContext());
	_descHelper->init(_simController->getContext());
	_versionController->init(_simController->getContext());
	_repository->init(_notifier, _simAccess, _descHelper, _simController->getContext(), _numberGenerator);

	_view->setupEditors(_simController);

	addRandomEnergy(config.energy);

	_view->refresh();
}

void MainController::onSaveSimulation(string const& filename)
{
	_serializationOperations.push_back({ SerializationOperation::Type::SaveToFile, filename });
	_serializer->serialize(_simController);
}

bool MainController::onLoadSimulation(string const & filename)
{
	auto origSimController = _simController;
	if (!SerializationHelper::loadFromFile<SimulationController*>(filename, [&](string const& data) { return _serializer->deserializeSimulation(data); }, _simController)) {
		return false;
	}
	delete origSimController;

	_simAccess->init(_simController->getContext());
	connectSimController();

	_model->setSimulationParameters(_simController->getContext()->getSimulationParameters());
	_model->setSymbolTable(_simController->getContext()->getSymbolTable());

	auto facade = ServiceLocator::getInstance().getService<ModelBuilderFacade>();
	_descHelper->init(_simController->getContext());
	_versionController->init(_simController->getContext());
	_repository->init(_notifier, _simAccess, _descHelper, _simController->getContext(), _numberGenerator);

	_view->setupEditors(_simController);
	_view->refresh();
	return true;
}

void MainController::onUpdateSimulationParametersForRunningSimulation()
{
	_simController->getContext()->setSimulationParameters(_model->getSimulationParameters());
}

int MainController::getTimestep() const
{
	return _simController->getTimestep();
}

SimulationConfig MainController::getSimulationConfig() const
{
	auto context = _simController->getContext();
	return{ context->getMaxThreads(), context->getGridSize(), context->getSpaceProperties()->getSize() };
}

void MainController::connectSimController() const
{
	connect(_simController, &SimulationController::nextTimestepCalculated, [this]() {
		_view->getInfoController()->increaseTimestep();
	});
}

void MainController::addRandomEnergy(double amount)
{
	double maxEnergyPerCell = _simController->getContext()->getSimulationParameters()->cellMinEnergy;
	_repository->addRandomParticles(amount, maxEnergyPerCell);
	Q_EMIT _notifier->notify({
		Receiver::DataEditor,
		Receiver::Simulation,
		Receiver::VisualEditor,
		Receiver::ActionController
	}, UpdateDescription::All);

}

void MainController::serializationFinished()
{
	for (SerializationOperation operation : _serializationOperations) {
		if (operation.type == SerializationOperation::Type::SaveToFile) {
			SerializationHelper::saveToFile(operation.filename, [&]() { return _serializer->retrieveSerializedSimulation(); });
		}
	}
	_serializationOperations.clear();
}

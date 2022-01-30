#include "EditorController.h"

#include <imgui.h>

#include "Base/Math.h"
#include "EngineInterface/SimulationController.h"
#include "EngineInterface/InspectedEntityIds.h"
#include "EngineInterface/DescriptionHelper.h"
#include "Viewport.h"
#include "StyleRepository.h"
#include "EditorModel.h"
#include "SelectionWindow.h"
#include "ManipulatorWindow.h"
#include "CreatorWindow.h"
#include "MultiplierWindow.h"
#include "SymbolsWindow.h"

_EditorController::_EditorController(SimulationController const& simController, Viewport const& viewport)
    : _simController(simController)
    , _viewport(viewport)
{
    _editorModel = std::make_shared<_EditorModel>(_simController);
    _selectionWindow = std::make_shared<_SelectionWindow>(_editorModel);
    _manipulatorWindow = std::make_shared<_ManipulatorWindow>(_editorModel, _simController, _viewport);
    _creatorWindow = std::make_shared<_CreatorWindow>(_editorModel, _simController, _viewport);
    _multiplierWindow = std::make_shared<_MultiplierWindow>(_editorModel, _simController, _viewport);
    _symbolsWindow = std::make_shared<_SymbolsWindow>(_simController);
}

bool _EditorController::isOn() const
{
    return _on;
}

void _EditorController::setOn(bool value)
{
    _on = value;
}

void _EditorController::process()
{
    if (!_on) {
        return;
    }

    if (!_simController->isSimulationRunning()) {
        _selectionWindow->process();
        _manipulatorWindow->process();
        _creatorWindow->process();
        _multiplierWindow->process();
        _symbolsWindow->process();
    }
    if (!_creatorWindow->isOn()) {
        _editorModel->setDrawMode(false);
    }
    
    processSelectionRect();
    processInspectorWindows();

    if (!ImGui::GetIO().WantCaptureMouse) {
        auto mousePosImVec = ImGui::GetMousePos();
        RealVector2D mousePos{mousePosImVec.x, mousePosImVec.y};
        RealVector2D prevMousePosInt = _prevMousePosInt ? *_prevMousePosInt : mousePos;

        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            if (!_simController->isSimulationRunning()) {
                if (!_editorModel->isDrawMode()) {
                    selectEntities(mousePos, ImGui::GetIO().KeyCtrl);
                } else {
                    _creatorWindow->onDrawing();
                }
            }
        }
        if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
            if (!_simController->isSimulationRunning()) {
                if (!_editorModel->isDrawMode()) {
                    moveSelectedEntities(mousePos, prevMousePosInt, ImGui::GetIO().KeyShift);
                } else {
                    _creatorWindow->onDrawing();
                }
            } else {
                applyForces(mousePos, prevMousePosInt);
            }
        }
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
            if (!_simController->isSimulationRunning() && !_editorModel->isDrawMode()) {
                createSelectionRect(mousePos);
            }
        }
        if (ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
            if (!_simController->isSimulationRunning() && !_editorModel->isDrawMode()) {
                resizeSelectionRect(mousePos, prevMousePosInt);
            }
        }

        _prevMousePosInt = mousePos;
    }

    if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
        if (_editorModel->isDrawMode()) {
            _creatorWindow->finishDrawing();
        }
    }
    if (ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
        if (!_simController->isSimulationRunning()) {
            removeSelectionRect();
        }
    }

    if (_simController->updateSelectionIfNecessary()) {
        _editorModel->update();
    }
}

SelectionWindow _EditorController::getSelectionWindow() const
{
    return _selectionWindow;
}

ManipulatorWindow _EditorController::getManipulatorWindow() const
{
    return _manipulatorWindow;
}

CreatorWindow _EditorController::getCreatorWindow() const
{
    return _creatorWindow;
}

MultiplierWindow _EditorController::getMultiplierWindow() const
{
    return _multiplierWindow;
}

SymbolsWindow _EditorController::getSymbolsWindow() const
{
    return _symbolsWindow;
}

bool _EditorController::areInspectionWindowsActive() const
{
    return !_inspectorWindows.empty();
}

void _EditorController::onCloseAllInspectorWindows()
{
    _inspectorWindows.clear();
}

bool _EditorController::isInspectionPossible() const
{
    return _manipulatorWindow->isInspectionPossible();
}

void _EditorController::onInspectEntities() const
{
    _manipulatorWindow->onInspectEntities();
}

bool _EditorController::isCopyingPossible() const
{
    return _manipulatorWindow->isCopyingPossible();
}

void _EditorController::onCopy()
{
    _manipulatorWindow->onCopy();
}

bool _EditorController::isPastingPossible() const
{
    return _manipulatorWindow->isPastingPossible();
}

void _EditorController::onPaste()
{
    _manipulatorWindow->onPaste();
}

void _EditorController::processSelectionRect()
{
    if (_selectionRect) {
        ImDrawList* draw_list = ImGui::GetBackgroundDrawList();
        auto startPos = _selectionRect->startPos;
        auto endPos = _selectionRect->endPos;
        draw_list->AddRectFilled({startPos.x, startPos.y}, {endPos.x, endPos.y}, Const::SelectionAreaFillColor);
        draw_list->AddRect({startPos.x, startPos.y}, {endPos.x, endPos.y}, Const::SelectionAreaBorderColor, 0, 0, 1.0f);
    }
}

void _EditorController::processInspectorWindows()
{
    //new entities to inspect
    newEntitiesToInspect(_editorModel->fetchEntitiesToInspect());

    //process inspector windows
    for (auto const& inspectorWindow : _inspectorWindows) {
        inspectorWindow->process();
    }

    //inspector windows closed?
    std::vector<InspectorWindow> inspectorWindows;
    std::vector<CellOrParticleDescription> inspectedEntities;
    for (auto const& inspectorWindow : _inspectorWindows) {
        if (!inspectorWindow->isClosed()) {
            inspectorWindows.emplace_back(inspectorWindow);

            auto id = inspectorWindow->getId();
            inspectedEntities.emplace_back(_editorModel->getInspectedEntity(id));
        }
    }
    _inspectorWindows = inspectorWindows;
    _editorModel->setInspectedEntities(inspectedEntities);

    //update inspected entities from simulation
    if (inspectedEntities.empty()) {
        return;
    }
    std::vector<uint64_t> entityIds;
    for (auto const& entity : inspectedEntities) {
        entityIds.emplace_back(DescriptionHelper::getId(entity));
    }
    auto inspectedData = _simController->getInspectedSimulationData(entityIds);
    auto newInspectedEntities = DescriptionHelper::getEntities(inspectedData);
    _editorModel->setInspectedEntities(newInspectedEntities);

    inspectorWindows.clear();
    for (auto const& inspectorWindow : _inspectorWindows) {
        if (_editorModel->existsInspectedEntity(inspectorWindow->getId())) {
            inspectorWindows.emplace_back(inspectorWindow);
        }
    }
    _inspectorWindows = inspectorWindows;
}

void _EditorController::newEntitiesToInspect(std::vector<CellOrParticleDescription> const& entities)
{
    if (entities.empty()) {
        return;
    }
    std::set<uint64_t> inspectedIds;
    for (auto const& inspectorWindow : _inspectorWindows) {
        inspectedIds.insert(inspectorWindow->getId());
    }
    auto origInspectedIds = inspectedIds;
    for (auto const& entity : entities) {
        inspectedIds.insert(DescriptionHelper::getId(entity));
    }

    std::vector<CellOrParticleDescription> newEntities;
    for (auto const& entity : entities) {
        if (origInspectedIds.find(DescriptionHelper::getId(entity)) == origInspectedIds.end()) {
            newEntities.emplace_back(entity);
        }
    }
    if (newEntities.empty()) {
        return;
    }
    if (inspectedIds.size() > Const::MaxInspectedEntities) {
        return;
    }
    RealVector2D center;
    int num = 0;
    for (auto const& entity : entities) {
        auto entityPos = _viewport->mapWorldToViewPosition(DescriptionHelper::getPos(entity));
        center += entityPos;
        ++num;
    }
    center = center / num;

    float maxDistanceFromCenter = 0;
    for (auto const& entity : entities) {
        auto entityPos = _viewport->mapWorldToViewPosition(DescriptionHelper::getPos(entity));
        auto distanceFromCenter = toFloat(Math::length(entityPos - center));
        maxDistanceFromCenter = std::max(maxDistanceFromCenter, distanceFromCenter);
    }
    auto viewSize = _viewport->getViewSize();
    auto factorX = maxDistanceFromCenter == 0 ? 1.0f : viewSize.x / maxDistanceFromCenter / 2.8f;
    auto factorY = maxDistanceFromCenter == 0 ? 1.0f : viewSize.y / maxDistanceFromCenter / 2.4f;

    for (auto const& entity : newEntities) {
        auto id = DescriptionHelper::getId(entity);
        _editorModel->addInspectedEntity(entity);
        auto entityPos = _viewport->mapWorldToViewPosition(DescriptionHelper::getPos(entity));
        auto windowPosX = (entityPos.x - center.x) * factorX + center.x;
        auto windowPosY = (entityPos.y - center.y) * factorY + center.y;
        windowPosX = std::min(std::max(windowPosX, 0.0f), toFloat(viewSize.x) - 200.0f) + 40.0f;
        windowPosY = std::min(std::max(windowPosY, 0.0f), toFloat(viewSize.y) - 200.0f) + 40.0f;
        _inspectorWindows.emplace_back(std::make_shared<_InspectorWindow>(
            _simController, _viewport, _editorModel, id, RealVector2D{windowPosX, windowPosY}));
    }
}

void _EditorController::selectEntities(RealVector2D const& viewPos, bool modifierKeyPressed)
{
    auto pos = _viewport->mapViewToWorldPosition({viewPos.x, viewPos.y});
    auto zoom = _viewport->getZoomFactor();
    if (!modifierKeyPressed) {
        _simController->switchSelection(pos, std::max(0.5f, 10.0f / zoom));
    } else {
        _simController->swapSelection(pos, std::max(0.5f, 10.0f / zoom));
    }

    _editorModel->update();
}

void _EditorController::moveSelectedEntities(
    RealVector2D const& viewPos,
    RealVector2D const& prevViewPos,
    bool modifierKeyPressed)
{
    auto start = _viewport->mapViewToWorldPosition({prevViewPos.x, prevViewPos.y});
    auto end = _viewport->mapViewToWorldPosition({viewPos.x, viewPos.y});
    auto zoom = _viewport->getZoomFactor();
    auto delta = end - start;

    ShallowUpdateSelectionData updateData;
    updateData.considerClusters = _editorModel->isRolloutToClusters() && !modifierKeyPressed;
    updateData.posDeltaX = delta.x;
    updateData.posDeltaY = delta.y;
    _simController->shallowUpdateSelectedEntities(updateData);
    _editorModel->update();
}

void _EditorController::applyForces(RealVector2D const& viewPos, RealVector2D const& prevViewPos)
{
    auto start = _viewport->mapViewToWorldPosition({prevViewPos.x, prevViewPos.y});
    auto end = _viewport->mapViewToWorldPosition({viewPos.x, viewPos.y});
    auto zoom = _viewport->getZoomFactor();
    _simController->applyForce_async(start, end, (end - start) / 50.0 * std::min(5.0f, zoom), 20.0f / zoom);
}

void _EditorController::createSelectionRect(RealVector2D const& viewPos)
{
    SelectionRect rect{viewPos, viewPos};
    _selectionRect = rect;
}

void _EditorController::resizeSelectionRect(RealVector2D const& viewPos, RealVector2D const& prevViewPos)
{
    _selectionRect->endPos = viewPos;
    auto startPos = _viewport->mapViewToWorldPosition(_selectionRect->startPos);
    auto endPos = _viewport->mapViewToWorldPosition(_selectionRect->endPos);
    auto topLeft = RealVector2D{std::min(startPos.x, endPos.x), std::min(startPos.y, endPos.y)};
    auto bottomRight = RealVector2D{std::max(startPos.x, endPos.x), std::max(startPos.y, endPos.y)};

    _simController->setSelection(topLeft, bottomRight);
    _editorModel->update();
}

void _EditorController::removeSelectionRect()
{
    _selectionRect = std::nullopt;
}
